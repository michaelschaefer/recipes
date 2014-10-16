#ifndef LIBRARY_HH
#define LIBRARY_HH


#include <QObject>
#include "database.hh"


class Library : public QObject {

    Q_OBJECT


signals:

    void statusBarMessage(QString);
    void updated();


public:

    static Library* instance();

    bool getFile(int fileId, Database::File& file);
    QList<int> getIngredientIdList(QString substring);
    bool getPath(int pathId, Database::Path& path);
    QStringList getPathList();
    QList<Database::Recipe> getRecipeList();
    bool insertOrUpdateFile(QString fileName, QString pathName, RecipeData& recipeData);
    bool rebuild();
    bool setPathList(QStringList& newPathList);
    bool update();


private:

    Library();
    Library(Library const&);
    void operator=(Library const&);

    bool insertFiles(QDir& dir, int pathId, int* nAdded = 0);
    bool insertPath(const QString& pathName, int* nAdded = 0);
    bool removePath(const QString& pathName, int* nRemoved = 0);
    bool updateFiles(QDir& dir, int pathId, int* nAdded = 0, int* nRemoved = 0);


    Database* m_database;    

};


#endif // LIBRARY_HH
