#ifndef LIBRARY_HH
#define LIBRARY_HH


#include <QObject>
#include "database.hh"


class Library : public QObject {

    Q_OBJECT


signals:

    void statusBarMessage(QString);


public:

    static Library* instance();

    Database::RecipeListType getRecipeList();
    QStringList getPathList();
    bool insertOrUpdateFile(QString fileName, QString pathName, RecipeData& recipeData);
    bool rebuild();
    bool setPathList(QStringList& newPathList);
    bool update();        


private:

    Library();
    Library(Library const&);
    void operator=(Library const&);

    bool addPath(const QString& pathName, int* nAdded = 0);
    bool addFiles(QDir& dir, int pathId, int* nAdded = 0);
    bool checkForRecipe(const QFileInfo& fileInfo, RecipeData& recipeData);
    bool removePath(const QString& pathName, int* nRemoved = 0);
    bool updateFiles(QDir& dir, int pathId, int* nAdded = 0, int* nRemoved = 0);


    Database* m_database;    

};


#endif // LIBRARY_HH
