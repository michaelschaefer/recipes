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
        m_uploadFile->close();
        emit uploadFinished(m_uploadFileList.takeFirst());
        if (m_uploadFileList.isEmpty() == true)
            emit entireUploadFinished();
        else
            upload();
    } else if (cmd == QFtp::List) {
        download();
    } else if (cmd == QFtp::Get) {
        m_downloadFile->close();
        emit downloadFinished(m_downloadFileList.takeFirst());
        if (m_downloadFileList.isEmpty() == true)
            emit entireDownloadFinished();
        else
            download();
    }
}


void FtpManager::download() {
    if (m_downloadFileList.isEmpty() == true)
        return;
    QString fileName = m_downloadFileList.first();
    QString pathName = QString("%1/%2").arg(m_downloadDir.absolutePath(), fileName);
    m_downloadFile = new QFile(pathName);
    m_downloadFile->open(QFile::WriteOnly);
    m_ftp->get(fileName, m_downloadFile);
}


void FtpManager::download(QDir dir) {
    m_downloadFileList.clear();
    m_downloadDir = dir;
    m_ftp->list();
}


FtpManager* FtpManager::instance() {
    static FtpManager instance;
    return &instance;
}


void FtpManager::listInfo(QUrlInfo urlInfo) {
    if (urlInfo.isFile() == false)
        return;

    QString fileName = urlInfo.name();
    int length = fileName.length();
    if (fileName.mid(length-4) != ".xml")
        return;
    else
        m_downloadFileList.append(fileName);
}


void FtpManager::login(QUrl url) {
    m_ftp = new QFtp(this);
    connect(m_ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(commandFinished(int, bool)));
    connect(m_ftp, SIGNAL(listInfo(QUrlInfo)), this, SLOT(listInfo(QUrlInfo)));

    m_url = url;
    if (m_url.isValid() == true) {
        m_ftp->connectToHost(m_url.host(), m_url.port());
    } else
        emit loginSuccess(false);
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


void FtpManager::upload() {
    if (m_uploadFileList.isEmpty() == true)
        return;
    QString pathName = m_uploadFileList.first();
    QString fileName = pathName.mid(pathName.lastIndexOf(QDir::separator()));
    m_uploadFile = new QFile(pathName);
    m_uploadFile->open(QFile::ReadOnly);
    m_ftp->put(m_uploadFile->readAll(), "./" + fileName);
}


void FtpManager::upload(QDir dir) {
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList() << "*.xml");    
    m_uploadFileList.clear();
    foreach (const QFileInfo& fileInfo, dir.entryInfoList()) {
        m_uploadFileList.append(fileInfo.absoluteFilePath());
    }
    upload();
}
