#ifndef LIBRARY_HH
#define LIBRARY_HH


#include <QObject>
#include "database.hh"


class Library : public QObject {

    Q_OBJECT


signals:

    void statusBarMessage(QString);


public:

    Library();

    Database::RecipeListType getRecipeList();
    QStringList getPathList();
    bool rebuild();
    bool setPathList(QStringList& newPathList);
    bool update();


private:

    bool addPath(const QString& pathName, int* nAdded = 0);
    bool addFiles(QDir& dir, int pathId, int* nAdded = 0);
    bool checkForRecipe(const QFileInfo& fileInfo, QString* headline = 0);    
    bool removePath(const QString& pathName, int* nRemoved = 0);
    bool updateFiles(QDir& dir, int pathId, int* nAdded = 0, int* nRemoved = 0);


    Database* m_database;

};


#endif // LIBRARY_HH
