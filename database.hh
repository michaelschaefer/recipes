#ifndef DATABASE_HH
#define DATABASE_HH


#include <QtSql>
#include <QDir>
#include <QObject>
#include "recipedata.hh"


class Database : public QObject {

    Q_OBJECT


public:

    typedef QPair<QString, int> RecipeType;
    typedef QList<RecipeType> RecipeListType;

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

    Database(QString fileName = QDir::currentPath() + QDir::toNativeSeparators("/library.sqlite"));

    void clear();
    void close();        
    bool getFile(int fileId, File& file);
    bool getFileId(const QString& fileName, int pathId, int* fileId = 0);
    QStringList getFileNameList(int pathId);
    bool getPath(int pathId, Path& path);
    bool getPathId(const QString& pathName, int* pathId = 0);
    QStringList getPathNameList();
    RecipeListType getRecipeList();
    bool insertFile(const QString& fileName, int pathId, RecipeData& recipeData, int* fileId = 0);
    bool insertPath(const QString& pathName, int* pathId = 0);
    bool isOpen();
    bool open();
    bool removeFile(const QString& fileName, int pathId);
    bool removePath(const QString& pathName, int* nRemoved);
    bool updateFile(int fileId, RecipeData& recipeData);


private:

    bool executeNoSelect(QString queryString);    
    bool init();

    QSqlDatabase m_database;
    QString m_databaseName;

};

#endif // DATABASE_HH
