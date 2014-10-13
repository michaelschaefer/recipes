#include <QDebug>
#include <QDir>
#include <QXmlStreamReader>
#include "library.hh"


Library::Library() {
    m_database = new Database();
}


Library *Library::instance() {
    static Library instance;
    return &instance;
}


bool Library::addFiles(QDir& dir, int pathId, int* nAdded) {
    int added = 0, fileId = 0;
    RecipeData recipeData;

    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList() << "*.xml");
    QFileInfoList fileInfoList = dir.entryInfoList();

    foreach (const QFileInfo& fileInfo, fileInfoList) {
        recipeData.clear();
        if (checkForRecipe(fileInfo, recipeData) == false)
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


bool Library::addPath(const QString& pathName, int* nAdded) {
    QDir dir(pathName);
    if (dir.exists() == false)
        return false;

    int pathId;
    if (m_database->insertPath(pathName, &pathId) == false)
        return false;
    else if (addFiles(dir, pathId, nAdded) == false)
        return false;

    return true;
}


bool Library::checkForRecipe(const QFileInfo &fileInfo, RecipeData& recipeData) {
    QFile file(fileInfo.absoluteFilePath());
    file.open(QFile::ReadOnly);
    QXmlStreamReader reader(&file);

    if (reader.readNextStartElement() == false) {
        file.close();
        return false;
    } else if (reader.name().toString() == "recipe") {        
        if (reader.readNextStartElement() == false) {
            file.close();
            return false;
        } else {
            QString headline = reader.readElementText();
            file.close();
            if (reader.hasError() == false) {
                recipeData.setHeadline(headline);
                return true;
            } else {
                return false;
            }
        }
    } else {
        file.close();
        return false;
    }
}


bool Library::getFile(int fileId, Database::File& file) {
    if (m_database->isOpen() == false)
        m_database->open();
    bool res = m_database->getFile(fileId, file);
    m_database->close();
    return res;
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


Database::RecipeListType Library::getRecipeList() {
    if (m_database->isOpen() == false)
        m_database->open();
    Database::RecipeListType recipeList = m_database->getRecipeList();
    m_database->close();
    return recipeList;
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
    if (m_database->isOpen() == false)
        m_database->close();

    if (m_database->removePath(pathName, nRemoved) == false)
        return false;

    m_database->close();
    return true;
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
                return false;
            } else
                removed += counter;
        }
    }

    foreach (const QString& path, newPathList) {
        if (currentPathList.contains(path) == false) {
            if (addPath(path, &counter) == false) {
                m_database->close();
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
                return false;
            }
            if (updateFiles(dir, pathId, &added, &removed) == false) {
                m_database->close();
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
    const QStringList& currentFileList = m_database->getFileNameList(pathId);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList() << "*.xml");
    const QStringList& newFileList = dir.entryList();
    const QFileInfoList& newFileInfoList = dir.entryInfoList();

    /*
     * remove files from the database that were
     * a) deleted or
     * b) overwritten by non-recipes
     */

    RecipeData recipeData;
    foreach (const QString& fileName, currentFileList) {
        if (newFileList.contains(fileName) == false) {
            if (m_database->removeFile(fileName, pathId) == true)
                removed++;
            else
                return false;
        } else {
            recipeData.clear();
            if (checkForRecipe(QFileInfo(dir, fileName), recipeData) == false) {
                if (m_database->removeFile(fileName, pathId) == true)
                    removed++;
                else
                    return false;
            }
        }
    }

    /*
     * add files from disk to the database that were
     */

    QString fileName;
    foreach (const QFileInfo& fileInfo, newFileInfoList) {
        fileName = fileInfo.fileName();
        if (currentFileList.contains(fileName) == false) {
            recipeData.clear();
            if (checkForRecipe(fileInfo, recipeData) == false)
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
