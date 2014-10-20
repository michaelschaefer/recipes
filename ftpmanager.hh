#ifndef FTPMANAGER_HH
#define FTPMANAGER_HH


#include <QtFtp/QFtp>
#include <QDir>
#include <QObject>
#include <QUrl>


class FtpManager : public QObject {

    Q_OBJECT


signals:

    void loginSuccess(bool);    
    void downloadFinished(QString);
    void entireDownloadFinished();
    void entireUploadFinished();
    void uploadFinished(QString);


public:

    static FtpManager* instance();

    void download(QDir dir);
    void login(QUrl url);    
    void upload(QDir dir);


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


    QDir m_downloadDir;
    QFtp* m_ftp;
    QFile* m_downloadFile;
    QFile* m_uploadFile;
    QStringList m_downloadFileList;
    QStringList m_uploadFileList;
    QUrl m_url;

};

#endif // FTPMANAGER_HH
