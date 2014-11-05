#include "exporter.hh"
#include "library.hh"


Library::Library() {
    m_database = new Database();
    m_settingsManager = SettingsManager::instance();

    m_synchronizer = new Synchronizer(this);    
    connect(m_synchronizer, SIGNAL(connectionFailed(QString)), this, SLOT(connectionFailed(QString)));
    connect(m_synchronizer, SIGNAL(entireDownloadFinished()), this, SLOT(downloadFinished()));
    connect(m_synchronizer, SIGNAL(entireUploadFinished()), this, SLOT(uploadFinished()));
    connect(m_synchronizer, SIGNAL(downloadFinished(QString)), this, SLOT(fileDownloaded(QString)));
    connect(m_synchronizer, SIGNAL(uploadFinished(QString)), this, SLOT(fileUploaded(QString)));
    connect(m_synchronizer, SIGNAL(synchronizationError(QString)), this, SLOT(synchronizationError(QString)));
    connect(m_synchronizer, SIGNAL(synchronizationFinished()), this, SLOT(synchronizationFinished()));

    msgDownloadFinished = trUtf8("(Download finished)");
    msgExportComplete = trUtf8("Export complete");
    msgExportingLibrary = trUtf8("Exporting library");
    msgExportProgress = trUtf8("(%1 of %2 finished)");
    msgFileDownloaded = trUtf8("(File %1 downloaded)");
    msgFileInserted = trUtf8("(File %1 inserted)");
    msgFileInsertedOrUpdated = trUtf8("(File %1 inserted or updated)");
    msgFileUpdated = trUtf8("(File %1 updated)");
    msgFileUploaded = trUtf8("(File %1 uploaded)");
    msgFileRemoved = trUtf8("(File %1 removed)");
    msgLibraryEmpty = trUtf8("Library is empty");
    msgLibraryUpdated = trUtf8("Library updated");
    msgRebuildComplete = trUtf8("Rebuild complete");
    msgRebuildingLibrary = trUtf8("Rebuilding library");
    msgSynchronizationFinished = trUtf8("Synchronization finished");
    msgSynchronizing = trUtf8("Synchronizing files");
    msgUpdateComplete = trUtf8("Update complete (%1 new, %2 removed)");
    msgUpdateFailed = trUtf8("Update failed (you may consider rebuilding the entire library)");
    msgUpdatingLibrary = trUtf8("Updating library");
    msgUploadFinished = trUtf8("(Upload finished)");
}


Library* Library::instance() {
    static Library instance;
    return &instance;
}


void Library::clear() {
    if (m_database->isOpen() == false)
        m_database->open();
    m_database->clear();
    emit statusBarMessage(msgLibraryEmpty);
    emit updated();

}


void Library::connectionFailed(QString error) {
    QString msg = trUtf8("Connection to server failed. Synchronization cannot be executed.");
    if (!error.isEmpty())
        msg += " " + trUtf8("Reason:") + "\n\n" + error;
    emit errorMessage(msg);
}


void Library::downloadFinished() {
    emit statusBarMessage(msgSynchronizing + " " + msgDownloadFinished);
    update();
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


QList<QPair<QString, QString> > Library::getFileHashList() {
    if (m_database->isOpen() == false)
        m_database->open();
    QList<QPair<QString, QString> > fileHashList = m_database->getFileHashList();
    m_database->close();
    return fileHashList;
}


QList<QFileInfo> Library::getFileInfoList() {
    QStringList fileNameList = getFileNameList();
    QList<QFileInfo> fileInfoList;
    QString dir = m_settingsManager->librarySettings().localPath;
    if (dir.isEmpty() == false) {
        foreach (const QString& fileName, fileNameList)
            fileInfoList.append(QFileInfo(dir + QDir::separator() + fileName));
    }

    return fileInfoList;
}


QStringList Library::getFileNameList() {
    if (m_database->isOpen() == false)
        m_database->open();
    QStringList fileNameList = m_database->getFileNameList();
    m_database->close();
    return fileNameList;
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
    QString libraryPath = m_settingsManager->librarySettings().localPath;
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
    QString pathName = m_settingsManager->librarySettings().localPath;
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


void Library::synchronizationError(QString error) {
    QString msg = trUtf8("Error while synchronizing library.");
    msg += " " + trUtf8("Reason:") + "\n\n" + error;
    emit errorMessage(msg);
}


void Library::synchronizationFinished() {
    emit statusBarMessage(msgSynchronizationFinished);
}


void Library::synchronize() {
    emit(statusBarMessage(msgSynchronizing));
    m_synchronizer->synchronize();
}


bool Library::update() {
    int added = 0, removed = 0;

    emit statusBarMessage(msgUpdatingLibrary);

    if (m_database->isOpen() == false)
        m_database->open();

    QString pathName = m_settingsManager->librarySettings().localPath;
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


void Library::uploadFinished() {
    emit statusBarMessage(msgSynchronizing + " " + msgUploadFinished);
}
