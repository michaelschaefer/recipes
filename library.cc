#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QXmlStreamReader>
#include "exporter.hh"
#include "library.hh"


Library::Library() {
    m_database = new Database();
    m_ftpManager = FtpManager::instance();
    connect(m_ftpManager, SIGNAL(entireDownloadFinished()), this, SLOT(update()));
    connect(m_ftpManager, SIGNAL(downloadFinished(QString)), this, SLOT(fileDownloaded(QString)));
    connect(m_ftpManager, SIGNAL(uploadFinished(QString)), this, SLOT(fileUploaded(QString)));
}


Library* Library::instance() {
    static Library instance;
    return &instance;
}


QString Library::msgExportComplete = trUtf8("Export complete");
QString Library::msgExportingLibrary = trUtf8("Exporting library");
QString Library::msgExportProgress = trUtf8("(%1 of %2 finished)");
QString Library::msgFileDownloaded = trUtf8("(File %1 downloaded)");
QString Library::msgFileInserted = trUtf8("(File %1 inserted)");
QString Library::msgFileInsertedOrUpdated = trUtf8("(File %1 inserted or updated)");
QString Library::msgFileUpdated = trUtf8("(File %1 updated)");
QString Library::msgFileUploaded = trUtf8("(File %1 uploaded)");
QString Library::msgFileRemoved = trUtf8("(File %1 removed)");
QString Library::msgLibraryEmpty = trUtf8("Library is empty");
QString Library::msgLibraryUpdated = trUtf8("Library updated");
QString Library::msgRebuildComplete = trUtf8("Rebuild complete");
QString Library::msgRebuildingLibrary = trUtf8("Rebuilding library");
QString Library::msgSynchronizing = trUtf8("Synchronizing files");
QString Library::msgUpdateComplete = trUtf8("Update complete (%1 new, %2 removed)");
QString Library::msgUpdateFailed = trUtf8("Update failed (you may consider rebuilding the entire library)");
QString Library::msgUpdatingLibrary = trUtf8("Updating library");


void Library::clear() {
    if (m_database->isOpen() == false)
        m_database->open();
    m_database->clear();
    emit statusBarMessage(msgLibraryEmpty);
    emit updated();

}


void Library::connectionFailed() {
    disconnect(m_ftpManager, SIGNAL(connectionFailed()), this, SLOT(connectionFailed()));
    emit errorMessage(trUtf8("Connection to FTP server failed. Synchronization cannot be executed."));
}


void Library::exportAsPdf(QString pathName) {
    emit statusBarMessage(msgExportingLibrary);
    Exporter* exporter = Exporter::instance();
    RecipeData recipeData;
    QList<QFileInfo> fileInfoList = getFileInfoList();
    if (fileInfoList.isEmpty() == true)
        return;
    int n = 0, N = fileInfoList.size();
    foreach (const QFileInfo& fileInfo, fileInfoList) {
        recipeData.clear();
        if (recipeData.fill(fileInfo.absoluteFilePath()) == false)
            continue;
        exporter->setRecipeData(recipeData);
        exporter->exportAsPdf(QString("%1%2%3").arg(pathName, QDir::separator(), fileInfo.fileName()), false);
        n++;
        emit statusBarMessage(msgExportingLibrary + " " + msgExportProgress.arg(n).arg(N));
    }
    emit statusBarMessage(msgExportComplete);
}


void Library::fileDownloaded(QString fileName) {
    emit statusBarMessage(msgSynchronizing + " " + msgFileDownloaded.arg(fileName));
}


void Library::fileUploaded(QString fileName) {
    emit statusBarMessage(msgSynchronizing + " " + msgFileUploaded.arg(fileName));
}


bool Library::getFile(int fileId, Database::File& file) {
    if (m_database->isOpen() == false)
        m_database->open();
    bool res = m_database->getFile(fileId, file);
    m_database->close();
    return res;
}


QList<QFileInfo> Library::getFileInfoList() {
    if (m_database->isOpen() == false)
        m_database->open();
    QStringList fileNameList = m_database->getFileNameList();
    m_database->close();

    QList<QFileInfo> fileInfoList;
    QString dir = QSettings().value("library/local/path").toString();
    if (dir.isEmpty() == false) {
        foreach (const QString& fileName, fileNameList)
            fileInfoList.append(QFileInfo(dir + QDir::separator() + fileName));
    }

    return fileInfoList;
}


QList<int> Library::getIngredientIdList(QString substring, Qt::CaseSensitivity caseSensitivity) {
    if (m_database->isOpen() == false)
        m_database->open();
    QList<int> ingredientIdList = m_database->getIngredientIdList(substring, caseSensitivity);
    m_database->close();
    return ingredientIdList;
}


QList<Database::Recipe> Library::getRecipeList() {
    if (m_database->isOpen() == false)
        m_database->open();
    QList<Database::Recipe> recipeList = m_database->getRecipeList();
    m_database->close();
    return recipeList;
}


bool Library::insertFiles(QDir& dir, int* nAdded) {
    int added = 0, fileId = 0;
    RecipeData recipeData;

    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList() << "*.xml");
    QFileInfoList fileInfoList = dir.entryInfoList();

    foreach (const QFileInfo& fileInfo, fileInfoList) {
        recipeData.clear();
        if (recipeData.fill(fileInfo.absoluteFilePath()) == false)
            continue;
        else if (m_database->insertFile(fileInfo.fileName(), recipeData, &fileId) == false)
            continue;
        else {
            added++;
            emit statusBarMessage(msgRebuildingLibrary + " " + msgFileInserted.arg(fileInfo.fileName()));
        }
    }

    if (nAdded != 0)
        *nAdded = added;

    return true;
}


bool Library::insertOrUpdateFile(QString absoluteFileName, RecipeData& recipeData) {
    int fileId = 0;

    // is library configured for data?
    int lastSeparatorIndex = absoluteFileName.lastIndexOf(QDir::separator());
    QString fileName = absoluteFileName.mid(lastSeparatorIndex + 1);
    QString pathName = absoluteFileName.mid(0, lastSeparatorIndex);
    QString libraryPath = QSettings().value("library/local/path").toString();
    if (libraryPath.isEmpty() || libraryPath != pathName)
        return false;

    if (m_database->isOpen() == false)
        m_database->open();

    if (m_database->getFileId(fileName, &fileId) == false) {
        m_database->close();
        return false;
    }

    if (fileId == -1) {
         if (m_database->insertFile(fileName, recipeData) == false) {
             m_database->close();
             return false;
         } else
             emit statusBarMessage(msgLibraryUpdated + " " + msgFileInserted.arg(fileName));
    } else
        if (m_database->updateFile(fileId, recipeData) == false) {
            m_database->close();
            return false;
        } else
            emit statusBarMessage(msgLibraryUpdated + " " + msgFileUpdated.arg(fileName));

    emit updated();
    m_database->close();
    return true;
}


bool Library::rebuild() {
    emit statusBarMessage(msgRebuildingLibrary);

    if (m_database->isOpen() == false)
        m_database->open();

    m_database->clear();
    QString pathName = QSettings().value("library/local/path").toString();
    if (pathName.isEmpty() == true) {
        emit statusBarMessage(msgLibraryEmpty);
        emit updated();
        return true;
    }

    QDir dir(pathName);
    bool ret = insertFiles(dir);
    m_database->close();
    emit statusBarMessage(msgRebuildComplete);
    emit updated();
    return ret;
}


void Library::synchronizeFiles(SyncState state) {
    static QMetaObject::Connection conn;

    if (state == Library::Connect) {
        connect(m_ftpManager, SIGNAL(connectionFailed()), this, SLOT(connectionFailed()));
        conn = connect(m_ftpManager, &FtpManager::connectionReady, [this] () { synchronizeFiles(Library::FetchRemote); });
        m_ftpManager->updateConnectionSettings();
        m_ftpManager->login();
    } else if (state == Library::FetchRemote) {
        disconnect(m_ftpManager, SIGNAL(connectionFailed()), this, SLOT(connectionFailed()));
        disconnect(conn);
        conn = connect(m_ftpManager, &FtpManager::fileListReady, [this] () { synchronizeFiles(Library::ExchangeData); });
        m_ftpManager->fetchFileList();
    } else if (state == Library::ExchangeData) {
        disconnect(conn);
        QList<QUrlInfo> remoteInfoList = m_ftpManager->fileList();
        QList<QFileInfo> localInfoList = getFileInfoList();

        QList<QString> remoteNames;
        QList<qint64> remoteSizes;
        QList<QDateTime> remoteModified;
        QTime time;
        QDateTime dateTime;
        foreach (const QUrlInfo& urlInfo, remoteInfoList) {
            remoteNames.append(urlInfo.name());
            remoteSizes.append(urlInfo.size());

            dateTime = urlInfo.lastModified();
            time = dateTime.time();
            time.setHMS(time.hour(), time.minute(), 0);
            dateTime.setTime(time);
            remoteModified.append(dateTime);
        }

        QList<QString> localNames;
        QList<qint64> localSizes;
        QList<QDateTime> localModified;
        foreach (QFileInfo fileInfo, localInfoList) {
            localNames.append(fileInfo.fileName());
            localSizes.append(fileInfo.size());

            dateTime = fileInfo.lastModified();
            time = dateTime.time();
            time.setHMS(time.hour(), time.minute(), 0);
            dateTime.setTime(time);
            localModified.append(dateTime);
        }

        QStringList downloadList;
        QStringList uploadList;

        for (int i = 0; i < remoteNames.size(); ++i) {
            int index = localNames.indexOf(remoteNames[i]);
            if (index == -1)
                downloadList.append(remoteNames[i]);
            else {
                if (localModified[index] < remoteModified[i])
                    downloadList.append(remoteNames[i]);
            }
        }

        for (int i = 0; i < localNames.size(); ++i) {
            int index = remoteNames.indexOf(localNames[i]);
            if (index == -1)
                uploadList.append(localNames[i]);
            else {
                if (remoteModified[index] < localModified[i])
                    uploadList.append(localNames[i]);
            }
        }

        m_ftpManager->upload(uploadList);
        m_ftpManager->download(downloadList);
    }
}


bool Library::update() {
    int added = 0, removed = 0;

    emit statusBarMessage(msgUpdatingLibrary);

    if (m_database->isOpen() == false)
        m_database->open();

    QString pathName = QSettings().value("library/local/path").toString();
    if (pathName.isEmpty() == true) {
        clear();
        return true;
    }

    QDir dir(pathName);
    if (dir.exists() == true) {
        if (updateFiles(dir, &added, &removed) == false) {
            m_database->close();
            emit statusBarMessage(msgUpdateFailed);
            return false;
        }
    } else {
        clear();
        return true;
    }

    m_database->close();
    emit statusBarMessage(msgUpdateComplete.arg(added).arg(removed));
    emit updated();
    return true;
}


bool Library::updateFiles(QDir& dir, int* nAdded, int* nRemoved) {
    int added = 0, removed = 0;
    const QStringList& databaseFileList = m_database->getFileNameList();
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList() << "*.xml");
    const QStringList& discFileList = dir.entryList();
    const QFileInfoList& discFileInfoList = dir.entryInfoList();

    /*
     * remove files from the database that were
     * a) deleted or
     * b) overwritten by non-recipes
     */

    QString pathName;
    RecipeData recipeData;
    foreach (const QString& fileName, databaseFileList) {
        if (discFileList.contains(fileName) == false) {
            if (m_database->removeFile(fileName) == true) {
                removed++;
                emit statusBarMessage(msgUpdatingLibrary + " " + msgFileRemoved.arg(fileName));
            } else
                return false;
        } else {
            pathName = QString("%1%2%3").arg(dir.absolutePath(), QDir::separator(), fileName);
            recipeData.clear();
            if (recipeData.fill(pathName) == false) {
                if (m_database->removeFile(fileName) == true) {
                    removed++;
                    emit statusBarMessage(msgUpdatingLibrary + " " + msgFileRemoved.arg(fileName));
                } else
                    return false;
            }
        }
    }

    /*
     * add new files from disk to the database
     */

    QString fileName;
    foreach (const QFileInfo& fileInfo, discFileInfoList) {
        fileName = fileInfo.fileName();
        if (databaseFileList.contains(fileName) == false) {
            recipeData.clear();
            if (recipeData.fill(fileInfo.absoluteFilePath()) == false)
                continue;
            if (m_database->insertFile(fileName, recipeData) == true) {
                added++;
                emit statusBarMessage(msgUpdatingLibrary + " " + msgFileInsertedOrUpdated.arg(fileName));
            } else
                return false;
        }
    }

    if (nAdded != 0)
        *nAdded = added;
    if (nRemoved != 0)
        *nRemoved = removed;

    return true;
}
