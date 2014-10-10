#include <QDir>
#include "database.hh"


Database::Database(QString fileName) {
    m_databaseName = fileName;
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_databaseName);

    if (m_database.open() == true) {
        init();
        m_database.close();
    }
}


void Database::clear() {
    QSqlQuery query(m_database);
    QStringList tables = m_database.tables();
    foreach (const QString& tableName, tables) {
        if (query.exec("drop table " + tableName) == false)
            qDebug() << query.lastError();
    }

    init();
}


void Database::close() {
    if (m_database.isOpen() == true)
        m_database.close();
}


bool Database::executeNoSelect(QString queryString) {
    QSqlQuery query(m_database);
    query.prepare(queryString);
    return query.exec();
}


bool Database::getFileId(const QString& fileName, int pathId, int* fileId) {
    QSqlQuery query(m_database);
    query.prepare("select id from files where file=:file and path=:path");
    query.bindValue(":file", fileName);
    query.bindValue(":path", pathId);
    if (query.exec() == false || query.next() == false)
        return false;
    else {
        if (fileId != 0) {
            bool okay;
            *fileId = query.record().value(0).toInt(&okay);
            return okay;
        } else
            return true;
    }
}


QStringList Database::getFileList(int pathId) {
    QStringList fileList;
    QSqlQuery query(m_database);
    query.prepare("select file from files where path=:pathId");
    query.bindValue(":pathId", pathId);
    if (query.exec() == true) {
        while (query.next() == true)
            fileList.append(query.record().value(0).toString());
    }

    return fileList;
}


bool Database::getPathId(const QString& pathName, int* pathId) {
    QSqlQuery query(m_database);
    query.prepare("select id from paths where path=:path");
    query.bindValue(":path", pathName);
    if (query.exec() == false || query.next() == false)
        return false;
    else {
        if (pathId != 0) {
            bool okay;
            *pathId = query.record().value(0).toInt(&okay);
            return okay;
        } else
            return true;
    }
}


QStringList Database::getPathList() {
    QStringList pathList;

    QSqlQuery query(m_database);
    query.prepare("select path from paths");
    if (query.exec() == true) {
        while (query.next() == true) {
            pathList << query.record().value(0).toString();
        }
    }

    return pathList;
}


Database::RecipeListType Database::getRecipeList() {
    int pathId;
    RecipeListType recipeList;
    QSqlQuery query(m_database);
    QString headline;

    query.prepare("select headline, path from files");
    if (query.exec() == true) {
        while (query.next() == true) {
            QSqlRecord record = query.record();
            if (record.isEmpty() == true)
                continue;
            headline = record.value(0).toString();
            pathId = record.value(1).toInt();
            recipeList.append(RecipeType(headline, pathId));
        }
    } else {
        qDebug() << query.lastError();
    }

    return recipeList;
}


bool Database::init() {
    QString queryString;
    QStringList tables = m_database.tables();

    if (tables.contains("paths") == false) {
        queryString = "create table paths (id integer primary key, path varchar unique)";
        if (executeNoSelect(queryString) == false)
            return false;
    }

    if (tables.contains("files") == false) {
        queryString = "create table files (id integer primary key, path integer, file varchar, headline varchar)";
        if (executeNoSelect(queryString) == false)
            return false;

        queryString = "create unique index files_idx on files (file, path)";
        if (executeNoSelect(queryString) == false)
            return false;
    }

    return true;
}


bool Database::insertFile(const QString& fileName, int pathId, const QString& headline, int* fileId) {
    QSqlQuery query(m_database);
    query.prepare("insert into files (file, path, headline) values (:file, :path, :headline)");
    query.bindValue(":file", fileName);
    query.bindValue(":headline", headline);
    query.bindValue(":path", pathId);
    if (query.exec() == false) {
        if (query.lastError().number() == 19) {
            int id;
            if (getFileId(fileName, pathId, &id) == true) {
                query.prepare("update files set headline=:headline where id=:id");
                query.bindValue(":headline", headline);
                query.bindValue(":id", id);
                if (query.exec() == true) {
                    if (fileId != 0)
                        *fileId = id;
                    return true;
                } else
                    return false;
            } else
                return false;
        } else
            return false;
    } else {        
        if (fileId != 0)
            *fileId = query.lastInsertId().toInt();
        return true;
    }

    return true;
}


bool Database::insertPath(const QString& pathName, int* pathId) {
    QSqlQuery query(m_database);
    query.prepare("insert into paths (path) values (:path)");
    query.bindValue(":path", pathName);
    if (query.exec() == false) {
        if (query.lastError().number() == 19)
            if (pathId != 0)
                return getPathId(pathName, pathId);
            else
                return true;
        else
            return false;
    } else {
        if (pathId != 0)
            *pathId = query.lastInsertId().toInt();
        return true;
    }
}


bool Database::isOpen() {
    return m_database.isOpen();
}


bool Database::open() {
    return m_database.open();
}


bool Database::removeFile(const QString& fileName, int pathId) {
    QSqlQuery query(m_database);
    query.prepare("delete from files where file=:file and path=:pathId");
    query.bindValue(":file", fileName);
    query.bindValue(":pathId", pathId);
    return query.exec();
}


bool Database::removePath(const QString& pathName, int* nRemoved) {
    int pathId;
    if (getPathId(pathName, &pathId) == false)
        return false;

    QSqlQuery query(m_database);
    query.prepare("delete from files where path=:pathId");
    query.bindValue(":pathId", pathId);
    if (query.exec() == false)
        return false;
    else {
        if (nRemoved != 0)
            *nRemoved = query.numRowsAffected();
    }

    query.prepare("delete from paths where id=:pathId");
    query.bindValue(":pathId", pathId);
    if (query.exec() == false)
        return false;

    return true;
}
