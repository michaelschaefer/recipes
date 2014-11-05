#ifndef SYNCHRONIZER_HH
#define SYNCHRONIZER_HH


#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include "settingsmanager.hh"


class Synchronizer : public QObject {

    Q_OBJECT


signals:

    void connectionFailed();
    void connectionReady();
    void entireDownloadFinished();
    void entireUploadFinished();
    void downloadFinished(QString);
    void synchronizationFinished();
    void uploadFinished(QString);


public:    

    enum ExchangeDirection {
        Download,
        Upload
    };

    enum SynchronizationStage {
        ConnectionCheck,
        GetRemoteFileList,
        InspectRemoteFiles,
        SendFileList,
        ExchangeFiles,
        Finish
    };


    Synchronizer(QObject* parent = 0);

    void checkConnection(QUrl url);
    void synchronize();     


private slots:

    void connectionCheckFinished(QNetworkReply* reply);
    void duplicateDownloadFinished(QNetworkReply* reply);
    void exchangeFinished(QNetworkReply* reply);
    void fileListSent(QNetworkReply* reply);
    void getRemoteFileList(QNetworkReply* reply);
    void synchronize(SynchronizationStage stage);


private:

    void downloadDuplicates();
    void exchange(ExchangeDirection direction = Upload);
    void inspectDuplicate(QNetworkReply* reply);
    QString toLocalFileName(QString fileName);
    QByteArray prepareFileList(QList<QFileInfo> fileInfoList);
    QUrl prepareUrl(QString fileName);
    void synchronizationError(QNetworkReply::NetworkError error, SynchronizationStage stage);


    QNetworkAccessManager* m_networkManager;
    //QString m_localLibraryPath;
    QStringList m_downloadFileList;
    QList<QPair<QString, QString> > m_duplicatesFileList;
    QStringList m_remoteFileList;
    QStringList m_remoteHashList;
    QStringList m_uploadFileList;
    SettingsManager* m_settingsManager;
    SynchronizationStage m_currentStage;

};

#endif // SYNCHRONIZER_HH
