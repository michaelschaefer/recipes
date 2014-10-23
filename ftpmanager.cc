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
            emit connectionFailed();
        else
            m_ftp->login(m_url.userName(), m_url.password());
    } else if (cmd == QFtp::Login) {
        if (error == true)
            emit connectionFailed();
        else
            m_ftp->cd(m_url.path());
    } else if (cmd == QFtp::Cd) {
        if (error == true)
            emit connectionFailed();
        else
            emit connectionReady();
    } else if (cmd == QFtp::Put) {
        m_uploadFile->close();
        emit uploadFinished(m_uploadFileList.takeFirst());
        if (m_uploadFileList.isEmpty() == true)
            emit entireUploadFinished();
        else
            upload();
    } else if (cmd == QFtp::List) {
        emit fileListReady();
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
    if (m_downloadFileList.isEmpty() == true) {
        emit entireDownloadFinished();
        return;
    }

    QString fileName = m_downloadFileList.first();
    QString pathName = m_settings.value("library/local/path").toString();
    m_downloadFile = new QFile(QString("%1%2%3").arg(pathName, QDir::separator(), fileName));
    m_downloadFile->open(QFile::WriteOnly);
    m_ftp->get(fileName, m_downloadFile);
}


void FtpManager::download(QStringList fileNameList) {
    m_downloadFileList = fileNameList;
    download();
}


void FtpManager::fetchFileList() {
    m_urlInfoList.clear();
    m_ftp->list();
}


QList<QUrlInfo> FtpManager::fileList() {
    return m_urlInfoList;
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
    else {
        m_urlInfoList.append(urlInfo);
    }
}


void FtpManager::login() {
    if (m_ftp != 0) {
        m_ftp->close();
        delete m_ftp;
        m_ftp = 0;
    }

    m_ftp = new QFtp(this);
    connect(m_ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(commandFinished(int, bool)));
    connect(m_ftp, SIGNAL(listInfo(QUrlInfo)), this, SLOT(listInfo(QUrlInfo)));

    if (m_url.isValid() == true) {
        m_ftp->connectToHost(m_url.host(), m_url.port());
    } else
        emit connectionFailed();
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


void FtpManager::updateConnectionSettings() {
    m_url.setHost(m_settings.value("library/remote/address").toString());
    m_url.setPort(m_settings.value("library/remote/port").toInt());
    m_url.setUserName(m_settings.value("library/remote/userName").toString());
    m_url.setPassword(m_settings.value("library/remote/password").toString());
    m_url.setPath(m_settings.value("library/remote/path").toString());
}


void FtpManager::updateConnectionSettings(QUrl url) {
    m_url = url;
}


void FtpManager::upload() {
    if (m_uploadFileList.isEmpty() == true) {
        emit entireUploadFinished();
        return;
    }

    QString pathName = m_uploadFileList.first();
    QString fileName = pathName.mid(pathName.lastIndexOf(QDir::separator()));
    m_uploadFile = new QFile(pathName);
    m_uploadFile->open(QFile::ReadOnly);
    m_ftp->put(m_uploadFile->readAll(), "./" + fileName);
}


void FtpManager::upload(QStringList fileNameList) {
    QString pathName = m_settings.value("library/local/path").toString();
    m_uploadFileList.clear();
    foreach (const QString& fileName, fileNameList)
        m_uploadFileList.append(QString("%1%2%3").arg(pathName, QDir::separator(), fileName));
    upload();
}
