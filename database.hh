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
    bool getFileId(const QString& fileName, int* fileId = 0);
    QStringList getFileNameList();
    QList<int> getIngredientIdList(QString substring, Qt::CaseSensitivity caseSensitivity);
    QList<Recipe> getRecipeList();
    bool insertFile(const QString& fileName, RecipeData& recipeData, int* fileId = 0);
    bool isOpen();
    bool open();
    bool removeFile(const QString& fileName);    
    bool updateFile(int fileId, RecipeData& recipeData);


private:

    bool cleanIngredients();
    bool executeNoSelect(QString queryString);
    QList<int> getFileIdList();
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
