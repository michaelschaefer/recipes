#include <QCryptographicHash>
#include <QDir>
#include <QDebug>
#include "library.hh"
#include "synchronizer.hh"


Synchronizer::Synchronizer(QObject* parent)
    : QObject(parent)
{        
    m_networkManager = new QNetworkAccessManager(this);
    m_settingsManager = SettingsManager::instance();
}


void Synchronizer::checkConnection(QUrl url) {
    m_networkManager->clearAccessCache();
    connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(connectionCheckFinished(QNetworkReply*)));
    m_networkManager->get(QNetworkRequest(url));
}


void Synchronizer::connectionCheckFinished(QNetworkReply* reply) {    
    disconnect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(connectionCheckFinished(QNetworkReply*)));

    if (reply == 0) {
        emit connectionFailed(trUtf8("Unknown reason"));
        return;
    }

    QNetworkReply::NetworkError error = reply->error();

    if (reply->operation() == QNetworkAccessManager::GetOperation) {
        if (error == QNetworkReply::NoError) {
            emit connectionReady();
        } else if (error == QNetworkReply::ContentNotFoundError) {
            connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(connectionCheckFinished(QNetworkReply*)));
            m_networkManager->clearAccessCache();
            m_networkManager->put(QNetworkRequest(reply->url()), QByteArray());
        }
    } else if (reply->operation() == QNetworkAccessManager::PutOperation) {
        if (error == QNetworkReply::NoError) {
            emit connectionReady();
        } else
            emit connectionFailed(reply->errorString());
    }

    reply->deleteLater();
}


void Synchronizer::duplicateDownloadFinished(QNetworkReply* reply) {
    if (reply == 0) {
        emit synchronizationError(trUtf8("Unknown reason"));
        return;
    }

    QNetworkReply::NetworkError error = reply->error();

    if (error == QNetworkReply::NoError) {
        inspectDuplicate(reply->header(QNetworkRequest::LastModifiedHeader).toDateTime());
        m_duplicatesFileList.pop_front();
        downloadDuplicates();
    } else {
        emit synchronizationError(reply->errorString());
    }

    reply->deleteLater();
}


void Synchronizer::downloadDuplicates() {
    if (m_duplicatesFileList.isEmpty()) {
        disconnect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(duplicateDownloadFinished(QNetworkReply*)));
        synchronize(ExchangeFiles);
    } else {
        QString fileName = m_duplicatesFileList.first().first;
        m_networkManager->get(QNetworkRequest(prepareUrl(fileName)));
    }
}


void Synchronizer::exchange(ExchangeDirection direction) {
    if (direction == Upload) {
        if (m_uploadFileList.isEmpty()) {
            emit entireUploadFinished();
            exchange(Download);
        } else {
            QFile localFile(m_uploadFileList.first());
            if (localFile.open(QIODevice::ReadOnly)) {
                QByteArray uploadData = localFile.readAll();
                QString fileName = QFileInfo(localFile).fileName();
                m_networkManager->put(QNetworkRequest(prepareUrl(fileName)), uploadData);
                localFile.close();
            }
        }
    } else {
        if (m_downloadFileList.isEmpty()) {
            disconnect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(exchangeFinished(QNetworkReply*)));
            emit entireDownloadFinished();            
            synchronize(SendFileList);
        } else {
            m_networkManager->get(QNetworkRequest(prepareUrl(m_downloadFileList.first())));
        }
    }
}


void Synchronizer::exchangeFinished(QNetworkReply* reply) {
    if (reply == 0) {
        emit synchronizationError(trUtf8("Unknown reason"));
        return;
    }

    QNetworkReply::NetworkError error = reply->error();

    if (error == QNetworkReply::NoError) {
        if (reply->operation() == QNetworkAccessManager::GetOperation) {
            QString currentFileName = m_downloadFileList.takeFirst();
            QFile file(toLocalFileName(currentFileName));
            if (file.open(QIODevice::WriteOnly)) {
                file.write(reply->readAll());
                file.close();
            }            
            emit downloadFinished(currentFileName);
            exchange(Download);
        } else {
            QString currentFileName = m_uploadFileList.takeFirst();
            emit(uploadFinished(currentFileName));
            exchange(Upload);
        }        
    } else {
        emit synchronizationError(reply->errorString());
        reply->deleteLater();
        return;
    }

    reply->deleteLater();
}


void Synchronizer::fileListSent(QNetworkReply* reply) {
    disconnect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileListSent(QNetworkReply*)));

    if (reply == 0) {
        emit synchronizationError(trUtf8("Unknown reason"));
        return;
    }

    QNetworkReply::NetworkError error = reply->error();

    if (error != QNetworkReply::NoError) {
        emit synchronizationError(reply->errorString());
        reply->deleteLater();
        return;
    }

    reply->deleteLater();
    synchronize(Finish);
}


void Synchronizer::getRemoteFileList(QNetworkReply *reply) {
    disconnect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getRemoteFileList(QNetworkReply*)));

    if (reply == 0) {
        emit synchronizationError(trUtf8("Unknown reason"));
        return;
    }

    m_remoteFileList.clear();
    m_remoteHashList.clear();
    QNetworkReply::NetworkError error = reply->error();

    if (error == QNetworkReply::NoError) {
        QByteArray data = reply->readAll().trimmed();
        foreach (QString fileName, data.split('\n')) {
            fileName = fileName.trimmed();
            int index = fileName.lastIndexOf('/');
            m_remoteFileList.append(fileName.mid(0, index));
            if (index != -1)
                m_remoteHashList.append(fileName.mid(index + 1));
            else
                m_remoteHashList.append(QString());
        }
    } else if (error != QNetworkReply::ContentNotFoundError) {
        emit synchronizationError(reply->errorString());
        reply->deleteLater();
        return;
    }

    reply->deleteLater();
    synchronize(InspectRemoteFiles);
}


void Synchronizer::inspectDuplicate(QDateTime remoteLastModified) {
    QPair<QString, QString> remoteData = m_duplicatesFileList.first();
    QString remoteFileName = remoteData.first;
    QString localFileName = toLocalFileName(remoteFileName);
    QString remoteHash = remoteData.second;

    QFile file(localFileName);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        QString localHash = QString(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
        if (localHash != remoteHash) {            
            QDateTime localLastModified = QFileInfo(file).lastModified();
            if (localLastModified > remoteLastModified)
                m_uploadFileList.append(localFileName);
            else
                m_downloadFileList.append(remoteFileName);
        }
    } else {
        m_downloadFileList.append(remoteFileName);
    }
}


QByteArray Synchronizer::prepareFileList(QList<QFileInfo> fileInfoList) {
    QByteArray data;
    foreach (const QFileInfo& fileInfo, fileInfoList) {
        QFile file(fileInfo.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly)) {
            data.append(fileInfo.fileName());
            data.append('/');
            data.append(QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex());
            data.append('\n');
        }
    }

    return data;
}


QUrl Synchronizer::prepareUrl(QString fileName) {    
    SettingsManager::LibrarySettings librarySettings = m_settingsManager->librarySettings();
    QUrl url;

    url.setHost(librarySettings.remoteAddress);
    url.setPassword(librarySettings.remotePassword);
    url.setPath(librarySettings.remotePath + "/" + fileName);
    url.setPort(librarySettings.remotePort);
    url.setScheme(librarySettings.remoteProtocol);
    url.setUserName(librarySettings.remoteUserName);

    return url;
}


void Synchronizer::synchronize() {    
    synchronize(ConnectionCheck);
}


void Synchronizer::synchronize(SynchronizationStage stage) {    
    static Library* library = Library::instance();
    static QMetaObject::Connection conn;

    m_networkManager->clearAccessCache();

    if (stage == ConnectionCheck) {
        conn = connect(this, &Synchronizer::connectionReady, [this] () { synchronize(GetRemoteFileList); });
        checkConnection(prepareUrl("files.txt"));
    } else if (stage == GetRemoteFileList) {
        m_remoteFileList.clear();
        m_remoteHashList.clear();
        disconnect(conn);
        connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getRemoteFileList(QNetworkReply*)));        
        m_networkManager->get(QNetworkRequest(prepareUrl("files.txt")));
    } else if (stage == InspectRemoteFiles) {
        QStringList localFileList = library->getFileNameList();
        m_downloadFileList.clear();
        m_duplicatesFileList.clear();
        m_uploadFileList.clear();

        QString remoteFileName, remoteHash;
        for (int i = 0; i < m_remoteFileList.size(); ++i) {
            remoteFileName = m_remoteFileList[i];
            remoteHash = m_remoteHashList[i];
            if (localFileList.contains(remoteFileName))
                m_duplicatesFileList.append(QPair<QString, QString>(remoteFileName, remoteHash));
            else
                m_downloadFileList.append(remoteFileName);
        }

        foreach (QString localFileName, localFileList) {
            if (!m_remoteFileList.contains(localFileName))
                m_uploadFileList.append(toLocalFileName(localFileName));
        }

        connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(duplicateDownloadFinished(QNetworkReply*)));
        downloadDuplicates();
    } else if (stage == ExchangeFiles) {
        connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(exchangeFinished(QNetworkReply*)));
        exchange();
    } else if (stage == SendFileList) {        
        library->update();
        QByteArray fileListData = prepareFileList(library->getFileInfoList());
        connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileListSent(QNetworkReply*)));
        m_networkManager->put(QNetworkRequest(prepareUrl("files.txt")), fileListData);
    } else if (stage == Finish) {
        emit(synchronizationFinished());
    }
}


QString Synchronizer::toLocalFileName(QString fileName) {
    fileName = fileName.mid(fileName.lastIndexOf(QDir::separator() + 1));
    return QString("%1%2%3").arg(m_settingsManager->librarySettings().localPath, QDir::separator(), fileName);
}
