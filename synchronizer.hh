#ifndef SYNCHRONIZER_HH
#define SYNCHRONIZER_HH


#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>


class Synchronizer : public QObject {

    Q_OBJECT


public:

    enum Stage {
        None,
        ConnectionCheck,
        GetRemoteFileList,
        InspectRemoteFiles
    };


    Synchronizer(QObject* parent = 0);

    void synchronize();


private slots:

    void finished(QNetworkReply* reply);
    void synchronizeInternal();


private:

    void download(QString fileName);
    void getRemoteFileList(QNetworkReply* reply);
    QUrl prepareUrl(QString fileName);


    QNetworkAccessManager m_networkManager;
    QStringList m_remoteFileList;
    Stage m_currentStage;

};

#endif // SYNCHRONIZER_HH
