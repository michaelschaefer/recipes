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


bool Database::cleanIngredients() {
    QSqlQuery query(m_database);
    query.prepare("delete from ingredients where id in (select * from (select ingredientId from (select ingredients.id as ingredientId, ifnull(ingredientAssignment.id, 0) as ingredientAssignmentId from ingredients left join ingredientAssignment on ingredientAssignment.ingredient = ingredients.id) where ingredientAssignmentId = 0))");
    return query.exec();
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


bool Database::getFile(int fileId, File& file) {
    QSqlQuery query(m_database);
    query.prepare("select id, file, headline from files where id=:fileId");
    query.bindValue(":fileId", fileId);
    if (query.exec() == false || query.next() == false)
        return false;
    else {
        QSqlRecord record = query.record();
        file.id = record.value(0).toInt();        
        file.fileName = record.value(1).toString();
        file.headline = record.value(2).toString();
        return true;
    }
}


bool Database::getFileId(const QString& fileName, int* fileId) {
    QSqlQuery query(m_database);
    query.prepare("select id from files where file=:fileName");
    query.bindValue(":fileName", fileName);
    if (query.exec() == false)
        return false;
    else {
        if (fileId != 0) {
            if (query.next() == false) {
                *fileId = -1;
                return true;
            } else {
                bool okay;
                *fileId = query.record().value(0).toInt(&okay);
                return okay;
            }
        } else
            return true;
    }
}


QList<int> Database::getFileIdList() {
    QList<int> idList;
    QSqlQuery query(m_database);    
    query.prepare("select id from files");
    if (query.exec() == false)
        return idList;
    while (query.next() == true)
        idList << query.record().value(0).toInt();
    return idList;
}


QStringList Database::getFileNameList() {
    QStringList fileList;
    QSqlQuery query(m_database);    
    query.prepare("select file from files");
    if (query.exec() == true) {
        while (query.next() == true)
            fileList.append(query.record().value(0).toString());
    }    

    return fileList;
}


bool Database::getIngredientId(QString ingredientName, int* ingredientId) {
    QSqlQuery query(m_database);
    query.prepare("select id from ingredients where ingredient=:ingredientName");
    query.bindValue(":ingredientName", ingredientName);
    if (query.exec() == false)
        return false;
    if (ingredientId == 0)
        return true;
    if (query.next() == false) {
        *ingredientId = -1;
        return true;
    } else {
        bool okay;
        *ingredientId = query.record().value(0).toInt(&okay);
        return okay;
    }
}


QList<QPair<QString, int> > Database::getIngredientList(int fileId) {
    QList<QPair<QString, int> > ingredientList;
    QSqlQuery query(m_database);
    query.prepare("select ingredients.ingredient, ingredients.id from ingredientAssignment left join ingredients on ingredients.id = ingredientAssignment.ingredient where ingredientAssignment.file = :fileId");
    query.bindValue(":fileId", fileId);
    if (query.exec() == true) {
        int ingredientId;
        QString ingredientName;
        while (query.next() == true) {
            QSqlRecord record = query.record();
            ingredientName = record.value(0).toString();
            ingredientId = record.value(1).toInt();
            ingredientList.append(QPair<QString, int>(ingredientName, ingredientId));
        }
    }
    return ingredientList;
}


QList<int> Database::getIngredientIdList(int fileId) {
    QList<int> ingredientIdList;
    QSqlQuery query(m_database);
    query.prepare("select ingredient from ingredientAssignment where file=:fileId");
    query.bindValue(":fileId", fileId);
    if (query.exec() == false)
        return ingredientIdList;
    while (query.next() == true)
        ingredientIdList.append(query.record().value(0).toInt());
    return ingredientIdList;
}


QList<int> Database::getIngredientIdList(QString substring, Qt::CaseSensitivity caseSensitivity) {
    QList<int> ingredientIdList;
    QSqlQuery query(m_database);
    query.prepare("select id from ingredients where ingredient like :query");
    query.bindValue(":query", QString("%%1%").arg(substring));

    bool ret;
    if (caseSensitivity == Qt::CaseSensitive) {
        QSqlQuery pragmaQuery(m_database);
        pragmaQuery.prepare("pragma case_sensitive_like=ON");
        pragmaQuery.exec();

        ret = query.exec();

        pragmaQuery.prepare("pragma case_sensitive_like=OFF");
        pragmaQuery.exec();
    } else
        ret = query.exec();

    if (ret == false)
        return ingredientIdList;

    while (query.next() == true) {
        ingredientIdList.append(query.record().value(0).toInt());
    }
    return ingredientIdList;
}


QList<Database::Recipe> Database::getRecipeList() {    
    QList<Recipe> recipeList;

    QSqlQuery query(m_database);    
    query.prepare("select id, headline from files");
    if (query.exec() == true) {
        while (query.next() == true) {
            QSqlRecord record = query.record();
            if (record.isEmpty() == true)
                continue;
            Recipe recipe;
            recipe.fileId = record.value(0).toInt();
            recipe.headline = record.value(1).toString();
            recipe.ingredientIdList = getIngredientIdList(recipe.fileId);
            recipeList.append(recipe);
        }
    }

    return recipeList;
}


bool Database::init() {
    QString queryString;
    QStringList tables = m_database.tables();    

    if (tables.contains("files") == false) {
        queryString = "create table files (id integer primary key, file varchar, headline varchar)";
        if (executeNoSelect(queryString) == false)
            return false;
    }

    if (tables.contains("ingredients") == false) {
        queryString = "create table ingredients (id integer primary key, ingredient varchar unique)";
        if (executeNoSelect(queryString) == false)
            return false;
    }

    if (tables.contains("ingredientAssignment") == false) {
        queryString = "create table ingredientAssignment (id integer primary key, ingredient integer, file integer)";
        if (executeNoSelect(queryString) == false)
            return false;

        queryString = "create unique index ingredientAssignment_idx on ingredientAssignment (ingredient, file)";
        if (executeNoSelect(queryString) == false)
            return false;
    }

    return true;
}


bool Database::insertFile(const QString& fileName, RecipeData& recipeData, int* fileId) {
    int id;
    QSqlQuery query(m_database);    
    query.prepare("insert into files (file, headline) values (:fileName, :headline)");
    query.bindValue(":fileName", fileName);
    query.bindValue(":headline", recipeData.headline());
    if (query.exec() == false) {
        if (query.lastError().number() == 19) {
            if (getFileId(fileName, &id) == true) {
                query.prepare("update files set headline=:headline where id=:id");
                query.bindValue(":headline", recipeData.headline());
                query.bindValue(":id", id);
                if (query.exec() == false)
                    return false;
            } else
                return false;
        } else
            return false;
    } else
        id = query.lastInsertId().toInt();

    if (fileId != 0)
        *fileId = id;

    return insertIngredients(id, recipeData);
}


bool Database::insertIngredient(QString ingredientName, int* ingredientId) {
    if (getIngredientId(ingredientName, ingredientId) == false)
        return false;

    if (ingredientId != 0 && *ingredientId != -1)
        return true;

    QSqlQuery query(m_database);
    query.prepare("insert into ingredients (ingredient) values (:ingredientName)");
    query.bindValue(":ingredientName", ingredientName);
    if (query.exec() == false)
        return false;

    if (ingredientId != 0)
        *ingredientId = query.lastInsertId().toInt();

    return true;
}


bool Database::insertIngredientAssignment(int fileId, int ingredientId) {
    QSqlQuery query(m_database);
    query.prepare("insert or ignore into ingredientAssignment (ingredient, file) values (:ingredientId, :fileId)");
    query.bindValue(":ingredientId", ingredientId);
    query.bindValue(":fileId", fileId);
    return query.exec();
}


bool Database::insertIngredients(int fileId, RecipeData& recipeData) {
    int ingredientId;
    foreach (const QString& ingredientName, recipeData.ingredientNameList()) {
        if (insertIngredient(ingredientName, &ingredientId) == false)
            return false;
        if (insertIngredientAssignment(fileId, ingredientId) == false)
            return false;
    }

    return true;
}


bool Database::isOpen() {
    return m_database.isOpen();
}


bool Database::open() {
    return m_database.open();
}


bool Database::removeFile(const QString& fileName) {
    QSqlQuery query(m_database);
    query.prepare("delete from files where file=:fileName");
    query.bindValue(":fileName", fileName);
    return query.exec();
}


bool Database::removeIngredients(int fileId) {
    QSqlQuery query(m_database);
    query.prepare("delete from ingredientAssignment where file=:fileId");
    query.bindValue(":fileId", fileId);
    return query.exec();
}


bool Database::removeIngredientAssignment(int fileId, int ingredientId) {
    QSqlQuery query(m_database);
    query.prepare("delete from ingredientAssignment where file=:fileId and ingredient=:ingredientId");
    query.bindValue(":fileId", fileId);
    query.bindValue(":ingredientId", ingredientId);
    return query.exec();
}


bool Database::updateFile(int fileId, RecipeData& recipeData) {
    QSqlQuery query(m_database);
    query.prepare("update files set headline=:headline where id=:fileId");
    query.bindValue(":fileId", fileId);
    query.bindValue(":headline", recipeData.headline());
    if (query.exec() == false)
        return false;

    QStringList newIngredientList = recipeData.ingredientNameList();
    QList<QPair<QString, int> > currentIngredientList = getIngredientList(fileId);
    QStringList currentIngredientNameList;
    QList<int> currentIngredientIdList;

    QPair<QString, int> ingredient;
    foreach (ingredient, currentIngredientList) {
        currentIngredientNameList << ingredient.first;
        currentIngredientIdList << ingredient.second;
        if (newIngredientList.contains(ingredient.first) == false)
            removeIngredientAssignment(fileId, ingredient.second);
    }

    int ingredientId;
    foreach (const QString& ingredientName, newIngredientList) {
        if (currentIngredientNameList.contains(ingredientName) == false) {
            if (insertIngredient(ingredientName, &ingredientId) == false)
                return false;
            if (insertIngredientAssignment(fileId, ingredientId) == false)
                return false;
        }
    }

    cleanIngredients();
    return true;
}
