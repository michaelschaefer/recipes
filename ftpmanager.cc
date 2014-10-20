#include <QDebug>
#include <QMessageBox>
#include "ftpmanager.hh"


FtpManager::FtpManager() {
    m_ftp = 0;
}


void FtpManager::commandFinished(int, bool error) {
    QFtp::Command cmd = m_ftp->currentCommand();
    if (cmd == QFtp::ConnectToHost) {
        if (error == true)
            emit loginSuccess(false);
        else
            m_ftp->login(m_url.userName(), m_url.password());
    } else if (cmd == QFtp::Login) {
        if (error == true)
            emit loginSuccess(false);
        else
            m_ftp->cd(m_url.path());
    } else if (cmd == QFtp::Cd) {
        emit loginSuccess(error == false);
    } else if (cmd == QFtp::Put) {
    }
}


void FtpManager::showError() {
    QString title = trUtf8("FTP error");
    QString text;

    QFtp::Command cmd = m_ftp->currentCommand();
    QString errorString = m_ftp->errorString();
    if (cmd == QFtp::ConnectToHost)
        text = trUtf8("Connection refused!\n\nError: %1").arg(errorString);
    else if (cmd == QFtp::Login)
        text = trUtf8("Login failed!\n\nError: %1").arg(errorString);

    QMessageBox::critical(0, title, text);
}


FtpManager* FtpManager::instance() {
    static FtpManager instance;
    return &instance;
}


void FtpManager::login(QUrl url) {
    m_ftp = new QFtp(this);
    connect(m_ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(commandFinished(int, bool)));

    m_url = url;
    if (m_url.isValid() == true) {
        m_ftp->connectToHost(m_url.host(), m_url.port());
    } else
        emit loginSuccess(false);
}


void FtpManager::upload(QDir dir) {
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList() << "*.xml");
    foreach (const QFileInfo& fileInfo, dir.entryInfoList()) {
        QFile file(fileInfo.absoluteFilePath());
        file.open(QFile::ReadOnly);
        m_ftp->put(file.readAll(), fileInfo.fileName());
        file.close();
    }
}
