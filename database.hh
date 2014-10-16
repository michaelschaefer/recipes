#ifndef DATABASE_HH
#define DATABASE_HH


#include <QtSql>
#include <QDir>
#include <QObject>
#include "recipedata.hh"


class Database : public QObject {

    Q_OBJECT


public:    

    struct File {
        QString fileName;
        QString headline;
        int id;
        int pathId;
    };

    struct Path {
        int id;
        QString pathName;
    };

    struct Recipe {
        QString headline;
        int fileId;
        QList<int> ingredientIdList;
    };

    Database(QString fileName = QDir::currentPath() + QDir::toNativeSeparators("/library.sqlite"));

    void clear();
    void close();
    bool getFile(int fileId, File& file);
    bool getFileId(const QString& fileName, int pathId, int* fileId = 0);
    QStringList getFileNameList(int pathId);
    QList<int> getIngredientIdList(QString substring);
    bool getPath(int pathId, Path& path);
    bool getPathId(const QString& pathName, int* pathId = 0);
    QStringList getPathNameList();
    QList<Recipe> getRecipeList();
    bool insertFile(const QString& fileName, int pathId, RecipeData& recipeData, int* fileId = 0);
    bool insertPath(const QString& pathName, int* pathId = 0);
    bool isOpen();
    bool open();
    bool removeFile(const QString& fileName, int pathId);
    bool removePath(const QString& pathName, int* nRemoved);
    bool updateFile(int fileId, RecipeData& recipeData);


private:

    bool cleanIngredients();
    bool executeNoSelect(QString queryString);
    QList<int> getFileIdList(int pathId);
    bool getIngredientId(QString ingredientName, int* ingredientId);
    QList<int> getIngredientIdList(int fileId);
    QList<QPair<QString, int> > getIngredientList(int fileId);
    bool insertIngredient(QString ingredientName, int* ingredientId);
    bool insertIngredientAssignment(int fileId, int ingredientId);
    bool insertIngredients(int fileId, RecipeData &recipeData);
    bool removeIngredientAssignment(int fileId, int ingredientId);
    bool removeIngredients(int fileId);
    bool init();


    QSqlDatabase m_database;
    QString m_databaseName;

};

#endif // DATABASE_HH
