#include <QDir>
#include <QDebug>
#include <QSettings>
#include "synchronizer.hh"


Synchronizer::Synchronizer(QObject* parent)
    : QObject(parent)
{
    connect(&m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
}


void Synchronizer::download(QString fileName) {
    QUrl url = prepareUrl(fileName);
    m_networkManager.get(QNetworkRequest(url));
}


void Synchronizer::finished(QNetworkReply* reply) {
    if (reply == 0)
        return;

    QNetworkAccessManager::Operation operation = reply->operation();
    //QNetworkReply::NetworkError error = reply->error();

    if (operation == QNetworkAccessManager::GetOperation) {

        if (m_currentStage == GetRemoteFileList) {
            getRemoteFileList(reply);
        }

    } else if (operation == QNetworkAccessManager::PutOperation) {
    }

    reply->deleteLater();
    synchronizeInternal();
}


void Synchronizer::getRemoteFileList(QNetworkReply *reply) {
    if (reply == 0)
        return;

    QNetworkReply::NetworkError error = reply->error();

    if (error == QNetworkReply::NoError) {
        QByteArray data = reply->readAll().trimmed();
        foreach (QString fileName, data.split('\n')) {
            m_remoteFileList.append(fileName);
        }
    } else {
        if (error == QNetworkReply::ContentNotFoundError) {
            qDebug() << "files.txt not found, returning empty list";
            m_remoteFileList.clear();
        }
    }
}


QUrl Synchronizer::prepareUrl(QString fileName) {
    QSettings settings;
    QUrl url;

    url.setHost(settings.value("library/remote/address").toString());
    url.setPassword(settings.value("library/remote/password").toString());
    url.setPath(settings.value("library/remote/path").toString() + "/" + fileName);
    url.setPort(settings.value("library/remote/port").toInt());
    url.setScheme("ftp");
    url.setUserName(settings.value("library/remote/userName").toString());

    return url;
}


void Synchronizer::synchronize() {
    m_currentStage = None;
    synchronizeInternal();
}


void Synchronizer::synchronizeInternal() {
    if (m_currentStage == None) {
        m_currentStage = GetRemoteFileList;
        download("files.txt");
    } else if (m_currentStage == GetRemoteFileList) {
        qDebug() << m_remoteFileList;
    }
}

