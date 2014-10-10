#include <QDebug>
#include <QDir>
#include <QXmlStreamReader>
#include "library.hh"


Library::Library() {
    m_database = new Database();
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


bool Library::addFiles(QDir& dir, int pathId, int* nAdded) {
    int added = 0, fileId = 0;
    QString headline;

    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList() << "*.xml");
    QFileInfoList fileInfoList = dir.entryInfoList();

    foreach (const QFileInfo& fileInfo, fileInfoList) {
        if (checkForRecipe(fileInfo, &headline) == false)
            continue;
        else if (m_database->insertFile(fileInfo.fileName(), pathId, headline, &fileId) == false)
            continue;
        else
            added++;
    }

    if (nAdded != 0)
        *nAdded = added;

    return true;
}


bool Library::checkForRecipe(const QFileInfo &fileInfo, QString* headline) {
    QFile file(fileInfo.absoluteFilePath());
    file.open(QFile::ReadOnly);
    QXmlStreamReader reader(&file);

    if (reader.readNextStartElement() == false) {
        file.close();
        return false;
    } else if (reader.name().toString() == "recipe") {
        if (headline != 0) {
            if (reader.readNextStartElement() == false) {
                file.close();
                return false;
            } else {
                *headline = reader.readElementText();
                return (reader.hasError() == false);
            }
        } else {
            file.close();
            return true;
        }
    } else {
        return false;
    }
}


QStringList Library::getPathList() {
    if (m_database->isOpen() == false)
        m_database->open();
    QStringList pathList = m_database->getPathList();
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


bool Library::rebuild() {
    emit statusBarMessage(trUtf8("rebuilding library..."));

    if (m_database->isOpen() == false)
        m_database->open();

    QStringList pathList = m_database->getPathList();
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

    QStringList currentPathList = m_database->getPathList();
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
    return true;
}


bool Library::update() {
    int added = 0, removed = 0;

    emit statusBarMessage(trUtf8("updating library..."));

    if (m_database->isOpen() == false)
        m_database->open();

    QStringList pathList = m_database->getPathList();
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
    return true;
}


bool Library::updateFiles(QDir& dir, int pathId, int* nAdded, int* nRemoved) {
    int added = 0, removed = 0;
    const QStringList& currentFileList = m_database->getFileList(pathId);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList() << "*.xml");
    const QStringList& newFileList = dir.entryList();
    const QFileInfoList& newFileInfoList = dir.entryInfoList();

    /*
     * remove files from the database that were
     * a) deleted or
     * b) overwritten by non-recipes
     */

    QString headline;
    foreach (const QString& fileName, currentFileList) {
        if (newFileList.contains(fileName) == false) {
            if (m_database->removeFile(fileName, pathId) == true)
                removed++;
            else
                return false;
        } else {
            if (checkForRecipe(QFileInfo(dir, fileName), &headline) == false) {
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
            if (checkForRecipe(fileInfo, &headline) == false)
                continue;
            if (m_database->insertFile(fileName, pathId, headline) == true)
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
