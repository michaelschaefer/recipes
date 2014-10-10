#ifndef DATABASE_HH
#define DATABASE_HH


#include <QtSql>
#include <QDir>
#include <QObject>


class Database : public QObject {

    Q_OBJECT


public:

    typedef QPair<QString, int> RecipeType;
    typedef QList<RecipeType> RecipeListType;

    Database(QString fileName = QDir::currentPath() + QDir::toNativeSeparators("/library.sqlite"));

    void clear();
    void close();    
    QStringList getFileList(int pathId);
    bool getFileId(const QString& fileName, int pathId, int* fileId = 0);
    bool getPathId(const QString& pathName, int* pathId = 0);
    QStringList getPathList();
    RecipeListType getRecipeList();
    bool insertFile(const QString& fileName, int pathId, const QString& headline, int* fileId = 0);
    bool insertPath(const QString& pathName, int* pathId = 0);
    bool isOpen();
    bool open();
    bool removeFile(const QString& fileName, int pathId);
    bool removePath(const QString& pathName, int* nRemoved);


private:

    bool executeNoSelect(QString queryString);    
    bool init();

    QSqlDatabase m_database;
    QString m_databaseName;

};

#endif // DATABASE_HH
