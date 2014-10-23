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

    void exportAsPdf(QString pathName);
    bool getFile(int fileId, Database::File& file);
    QList<QFileInfo> getFileInfoList();
    QList<int> getIngredientIdList(QString substring, Qt::CaseSensitivity caseSensitivity);
    QList<Database::Recipe> getRecipeList();
    bool insertOrUpdateFile(QString absoluteFileName, RecipeData& recipeData);
    bool rebuild();
    void synchronizeFiles(SyncState state = Library::Connect);


public slots:

    bool update();


private slots:

    void connectionFailed();
    void fileDownloaded(QString fileName);
    void fileUploaded(QString fileName);


private:

    Library();
    Library(Library const&);
    void operator=(Library const&);


    static QString msgExportComplete;
    static QString msgExportingLibrary;
    static QString msgExportProgress;
    static QString msgFileDownloaded;
    static QString msgFileInserted;
    static QString msgFileInsertedOrUpdated;
    static QString msgFileRemoved;
    static QString msgFileUpdated;
    static QString msgFileUploaded;
    static QString msgLibraryEmpty;
    static QString msgLibraryUpdated;
    static QString msgRebuildComplete;
    static QString msgRebuildingLibrary;
    static QString msgSynchronizing;
    static QString msgUpdatingLibrary;
    static QString msgUpdateFailed;
    static QString msgUpdateComplete;


    void clear();
    bool insertFiles(QDir& dir, int* nAdded = 0);
    bool updateFiles(QDir& dir, int* nAdded = 0, int* nRemoved = 0);


    Database* m_database;
    FtpManager* m_ftpManager;

};


#endif // LIBRARY_HH
