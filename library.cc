#include <QDebug>
#include <QDir>
#include <QXmlStreamReader>
#include "library.hh"


Library::Library() {
    m_database = new Database();
}


Library* Library::instance() {
    static Library instance;
    return &instance;
}


bool Library::getFile(int fileId, Database::File& file) {
    if (m_database->isOpen() == false)
        m_database->open();
    bool res = m_database->getFile(fileId, file);
    m_database->close();
    return res;
}


QList<int> Library::getIngredientIdList(QString substring) {
    if (m_database->isOpen() == false)
        m_database->open();
    QList<int> ingredientIdList = m_database->getIngredientIdList(substring);
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


QStringList Library::getPathList() {
    if (m_database->isOpen() == false)
        m_database->open();
    QStringList pathList = m_database->getPathNameList();
    m_database->close();
    return pathList;
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

    QStringList pathList = m_database->getPathNameList();
    m_database->clear();
    bool res = setPathList(pathList);
    m_database->close();
    emit statusBarMessage(trUtf8("rebuild complete"));    
    return res;
}


bool Library::removePath(const QString& pathName, int* nRemoved) {    
    return m_database->removePath(pathName, nRemoved);
}


bool Library::setPathList(QStringList& newPathList) {
    int added = 0, counter = 0, removed = 0;

    emit statusBarMessage(trUtf8("inserting paths into library..."));

    if (m_database->isOpen() == false)
        m_database->open();

    if (newPathList.isEmpty()) {
        m_database->clear();
        m_database->close();
        emit statusBarMessage(trUtf8("library is now empty"));
        return true;
    }

    QStringList currentPathList = m_database->getPathNameList();
    foreach (const QString& path, currentPathList) {
        if (newPathList.contains(path) == false) {
            if (removePath(path, &counter) == false) {
                m_database->close();
                emit statusBarMessage(trUtf8("inserting failed! Consider a library rebuild."));
                return false;
            } else
                removed += counter;
        }
    }

    foreach (const QString& path, newPathList) {
        if (currentPathList.contains(path) == false) {
            if (insertPath(path, &counter) == false) {
                m_database->close();
                emit statusBarMessage(trUtf8("inserting failed! Consider a library rebuild."));
                return false;
            } else
                added += counter;
        }
    }

    m_database->close();
    emit statusBarMessage(trUtf8("inserting complete (%1 new, %2 removed)").arg(added).arg(removed));
    emit updated();
    return true;
}


bool Library::update() {
    int added = 0, removed = 0;

    emit statusBarMessage(trUtf8("updating library..."));

    if (m_database->isOpen() == false)
        m_database->open();

    QStringList pathList = m_database->getPathNameList();
    foreach (const QString& pathName, pathList) {
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
            if (removePath(pathName, &removed) == false) {
                m_database->close();
                return false;
            }
        }
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
            pathName = QString("%1/%2").arg(dir.absolutePath()).arg(fileName);
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
