#ifndef LIBRARY_HH
#define LIBRARY_HH


#include <QObject>
#include "database.hh"
#include "ftpmanager.hh"


class Library : public QObject {

    Q_OBJECT


signals:

    void errorMessage(QString);
    void statusBarMessage(QString);
    void updated();


public:

    enum SyncState {
        Connect,
        ExchangeData,
        FetchRemote
    };


    static Library* instance();

    bool getFile(int fileId, Database::File& file);    
    QList<int> getIngredientIdList(QString substring, Qt::CaseSensitivity caseSensitivity);
    QList<Database::Recipe> getRecipeList();
    bool insertOrUpdateFile(QString absoluteFileName, RecipeData& recipeData);
    bool rebuild();
    void synchronize(SyncState state = Library::Connect);


public slots:

    bool update();


private slots:

    void connectionFailed();


private:

    Library();
    Library(Library const&);
    void operator=(Library const&);


    static QString msgFileInserted;
    static QString msgFileInsertedOrUpdated;
    static QString msgFileUpdated;
    static QString msgFileRemoved;
    static QString msgLibraryEmpty;
    static QString msgLibraryUpdated;
    static QString msgRebuildComplete;
    static QString msgRebuildingLibrary;
    static QString msgUpdatingLibrary;
    static QString msgUpdateFailed;
    static QString msgUpdateComplete;


    void clear();
    QList<QFileInfo> getFileInfoList();
    bool insertFiles(QDir& dir, int* nAdded = 0);    
    bool updateFiles(QDir& dir, int* nAdded = 0, int* nRemoved = 0);


    Database* m_database;    
    FtpManager* m_ftpManager;

};


#endif // LIBRARY_HH
