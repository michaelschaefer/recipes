#ifndef FTPMANAGER_HH
#define FTPMANAGER_HH


#include <QtFtp/QFtp>
#include <QDir>
#include <QObject>
#include <QSettings>
#include <QUrl>


class FtpManager : public QObject {

    Q_OBJECT


signals:

    void connectionFailed();
    void connectionReady();
    void downloadFinished(QString);
    void entireDownloadFinished();
    void entireUploadFinished();
    void fileListReady();
    void uploadFinished(QString);


public:

    static FtpManager* instance();

    void download(QStringList fileNameList);
    void fetchFileList();
    QList<QUrlInfo> fileList();
    void login();
    void updateConnectionSettings();
    void updateConnectionSettings(QUrl url);
    void upload(QStringList fileNameList);


private slots:

    void commandFinished(int, bool error);
    void listInfo(QUrlInfo urlInfo);


private:

    void download();
    void showError();
    void upload();


    FtpManager();
    FtpManager(FtpManager const&);
    void operator=(FtpManager const&);


    QFtp* m_ftp;
    QFile* m_downloadFile;
    QFile* m_uploadFile;
    QList<QUrlInfo> m_urlInfoList;
    QSettings m_settings;
    QStringList m_downloadFileList;
    QStringList m_uploadFileList;
    QUrl m_url;

};

#endif // FTPMANAGER_HH
