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


public:

    static FtpManager* instance();

    void login(QUrl url);
    void setDir(QDir dir);
    void upload(QDir dir);


private slots:

    void commandFinished(int, bool error);


private:

    void showError();


    FtpManager();
    FtpManager(FtpManager const&);
    void operator=(FtpManager const&);


    QFile* m_file;
    QFtp* m_ftp;
    QUrl m_url;

};

#endif // FTPMANAGER_HH
