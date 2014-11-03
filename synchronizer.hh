#ifndef SYNCHRONIZER_HH
#define SYNCHRONIZER_HH


#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>


class Synchronizer : public QObject {

    Q_OBJECT


signals:

    void entireDownloadFinished();
    void entireUploadFinished();
    void synchronizationFinished();


public:    

    enum SynchronizationStage {
        ConnectionCheck,
        GetRemoteFileList,
        InspectRemoteFiles,
        SendFileList,
        ExchangeFiles,
        Finish
    };


    Synchronizer(QObject* parent = 0);

    void synchronize();


private slots:

    void duplicateDownloadFinished(QNetworkReply* reply);
    void exchangeFinished(QNetworkReply* reply);
    void fileListSent(QNetworkReply* reply);
    void getRemoteFileList(QNetworkReply* reply);
    void synchronize(SynchronizationStage stage);


private:

    void downloadDuplicates();
    void exchange();
    void inspectDuplicate(QNetworkReply* reply);
    QString toLocalFileName(QString fileName);
    QUrl prepareUrl(QString fileName);
    void synchronizationError(QNetworkReply::NetworkError error, SynchronizationStage stage);


    QMetaObject::Connection m_conn;
    QNetworkAccessManager* m_networkManager;
    QString m_localLibraryPath;
    QStringList m_downloadFileList;
    QList<QPair<QString, QString> > m_duplicatesFileList;
    QStringList m_remoteFileList;
    QStringList m_remoteHashList;
    QStringList m_uploadFileList;
    SynchronizationStage m_currentStage;

};

#endif // SYNCHRONIZER_HH
