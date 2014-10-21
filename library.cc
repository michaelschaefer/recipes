#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QXmlStreamReader>
#include "library.hh"


Library::Library() {
    m_database = new Database();
    m_ftpManager = FtpManager::instance();

    auto connectionFailedSlot = [this] () {
        emit this->errorMessage(trUtf8("Connection to FTP server failed. Synchronization cannot be executed."));
    };
    connect(m_ftpManager, &FtpManager::connectionFailed, connectionFailedSlot);
    connect(m_ftpManager, &FtpManager::entireDownloadFinished, this, &Library::update);
}


Library* Library::instance() {
    static Library instance;
    return &instance;
}


void Library::clear() {
    if (m_database->isOpen() == false)
        m_database->open();
    m_database->clear();
    emit statusBarMessage(trUtf8("library is now empty"));
    emit updated();

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
    foreach (const QString& fileName, fileNameList) {
        fileInfoList.append(QFileInfo(fileName));
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


bool Library::getPath(int pathId, Database::Path& path) {
    if (m_database->isOpen() == false)
        m_database->open();
    bool res = m_database->getPath(pathId, path);
    m_database->close();
    return res;
}


QList<Database::Recipe> Library::getRecipeList() {
    if (m_database->isOpen() == false)
        m_database->open();
    QList<Database::Recipe> recipeList = m_database->getRecipeList();
    m_database->close();
    return recipeList;
}


bool Library::insertFiles(QDir& dir, int pathId, int* nAdded) {
    int added = 0, fileId = 0;
    RecipeData recipeData;

    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList() << "*.xml");
    QFileInfoList fileInfoList = dir.entryInfoList();

    foreach (const QFileInfo& fileInfo, fileInfoList) {
        recipeData.clear();
        if (recipeData.fill(fileInfo.absoluteFilePath()) == false)
            continue;
        else if (m_database->insertFile(fileInfo.fileName(), pathId, recipeData, &fileId) == false)
            continue;
        else
            added++;
    }

    if (nAdded != 0)
        *nAdded = added;

    return true;
}


bool Library::insertOrUpdateFile(QString fileName, QString pathName, RecipeData& recipeData) {
    int fileId = 0, pathId = 0;

    if (m_database->isOpen() == false)
        m_database->open();

    if (m_database->getPathId(pathName, &pathId) == false) {
        m_database->close();
        return false;
    } else if (pathId == -1)
        return true;

    if (m_database->getFileId(fileName, pathId, &fileId) == false) {
        m_database->close();
        return false;
    }

    if (fileId == -1) {
         if (m_database->insertFile(fileName, pathId, recipeData) == false) {
             m_database->close();
             return false;
         }
    } else
        if (m_database->updateFile(fileId, recipeData) == false) {
            m_database->close();
            return false;
        }

    emit updated();
    m_database->close();
    return true;
}


bool Library::insertPath(const QString& pathName, int* nAdded) {
    QDir dir(pathName);
    if (dir.exists() == false)
        return false;

    int pathId;
    if (m_database->insertPath(pathName, &pathId) == false)
        return false;
    else if (insertFiles(dir, pathId, nAdded) == false)
        return false;

    return true;
}


bool Library::rebuild() {
    emit statusBarMessage(trUtf8("rebuilding library..."));

    if (m_database->isOpen() == false)
        m_database->open();

    m_database->clear();
    QString pathName = QSettings().value("library/local/path").toString();
    if (pathName.isEmpty() == true) {
        emit statusBarMessage(trUtf8("library is now empty"));
        emit updated();
        return true;
    }

    bool ret = insertPath(pathName);
    m_database->close();
    emit statusBarMessage(trUtf8("rebuild complete"));
    emit updated();
    return ret;
}


void Library::synchronize(SyncState state) {
    static QMetaObject::Connection conn;
    if (state == Library::Connect) {
        conn = connect(m_ftpManager, &FtpManager::connectionReady, [this] () { synchronize(Library::FetchRemote); });
        m_ftpManager->updateConnectionSettings();
        m_ftpManager->login();
    } else if (state == Library::FetchRemote) {
        disconnect(conn);
        conn = connect(m_ftpManager, &FtpManager::fileListReady, [this] () { synchronize(Library::ExchangeData); });
        m_ftpManager->fetchFileList();
    } else if (state == Library::ExchangeData) {
        disconnect(conn);
        QList<QUrlInfo> remoteInfoList = m_ftpManager->fileList();
        QList<QFileInfo> localInfoList = getFileInfoList();

        QList<QString> remoteNames;
        QList<qint64> remoteSizes;
        QList<QDateTime> remoteModified;
        foreach (const QUrlInfo& urlInfo, remoteInfoList) {
            remoteNames.append(urlInfo.name());
            remoteSizes.append(urlInfo.size());
            remoteModified.append(urlInfo.lastModified());
        }

        QList<QString> localNames;
        QList<qint64> localSizes;
        QList<QDateTime> localModified;
        foreach (const QFileInfo& fileInfo, localInfoList) {
            localNames.append(fileInfo.fileName());
            localSizes.append(fileInfo.size());
            localModified.append(fileInfo.lastModified());
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

    emit statusBarMessage(trUtf8("updating library..."));

    if (m_database->isOpen() == false)
        m_database->open();

    QString pathName = m_database->getPathName();
    if (pathName.isEmpty() == true) {
        clear();
        return true;
    }

    QDir dir(pathName);
    if (dir.exists() == true) {
        int pathId;
        if (m_database->getPathId(pathName, &pathId) == false) {
            m_database->close();
            emit statusBarMessage(trUtf8("update failed! Consider a library rebuild."));
            return false;
        }
        if (updateFiles(dir, pathId, &added, &removed) == false) {
            m_database->close();
            emit statusBarMessage(trUtf8("update failed! Consider a library rebuild."));
            return false;
        }
    } else {
        clear();
        return true;
    }

    m_database->close();    
    emit statusBarMessage(trUtf8("update complete (%1 new, %2 removed)").arg(added).arg(removed));
    emit updated();
    return true;
}


bool Library::updateFiles(QDir& dir, int pathId, int* nAdded, int* nRemoved) {
    int added = 0, removed = 0;
    const QStringList& databaseFileList = m_database->getFileNameList(pathId);
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
            if (m_database->removeFile(fileName, pathId) == true)
                removed++;
            else
                return false;
        } else {
            pathName = QString("%1%2%3").arg(dir.absolutePath(), QDir::separator(), fileName);
            recipeData.clear();
            if (recipeData.fill(pathName) == false) {
                if (m_database->removeFile(fileName, pathId) == true)
                    removed++;
                else
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
            if (m_database->insertFile(fileName, pathId, recipeData) == true)
                added++;
            else
                return false;
        }
    }

    if (nAdded != 0)
        *nAdded = added;
    if (nRemoved != 0)
        *nRemoved = removed;

    return true;
}
