#ifndef LIBRARY_HH
#define LIBRARY_HH


#include <QNetworkReply>
#include <QObject>
#include "database.hh"
#include "settingsmanager.hh"
#include "synchronizer.hh"


class Library : public QObject {

    Q_OBJECT


signals:

    void errorMessage(QString);
    void statusBarMessage(QString);
    void updated();


public:

    static Library* instance();

    void exportAsPdf(QString pathName);
    bool getFile(int fileId, Database::File& file);
    QList<QFileInfo> getFileInfoList();
    QStringList getFileNameList();
    QList<int> getIngredientIdList(QString substring, Qt::CaseSensitivity caseSensitivity);
    QList<Database::Recipe> getRecipeList();
    bool insertOrUpdateFile(QString absoluteFileName, RecipeData& recipeData);    
    bool rebuild();
    void synchronize();


public slots:

    bool update();


private slots:

    void connectionFailed();
    void downloadFinished();
    void fileDownloaded(QString fileName);
    void fileUploaded(QString fileName);
    void synchronizationFinished();
    void uploadFinished();


private:

    Library();
    Library(Library const&);
    void operator=(Library const&);


    QString msgDownloadFinished;
    QString msgExportComplete;
    QString msgExportingLibrary;
    QString msgExportProgress;
    QString msgFileDownloaded;
    QString msgFileInserted;
    QString msgFileInsertedOrUpdated;
    QString msgFileRemoved;
    QString msgFileUpdated;
    QString msgFileUploaded;
    QString msgLibraryEmpty;
    QString msgLibraryUpdated;
    QString msgRebuildComplete;
    QString msgRebuildingLibrary;
    QString msgSynchronizationFinished;
    QString msgSynchronizing;
    QString msgUpdatingLibrary;
    QString msgUpdateFailed;
    QString msgUpdateComplete;
    QString msgUploadFinished;


    void clear();
    bool insertFiles(QDir& dir, int* nAdded = 0);
    bool updateFiles(QDir& dir, int* nAdded = 0, int* nRemoved = 0);


    Database* m_database;    
    SettingsManager* m_settingsManager;
    Synchronizer* m_synchronizer;

};


#endif // LIBRARY_HH
