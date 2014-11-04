#include <QCryptographicHash>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include "library.hh"
#include "synchronizer.hh"


Synchronizer::Synchronizer(QObject* parent)
    : QObject(parent)
{    
    m_localLibraryPath = QSettings().value("library/local/path").toString();
    m_networkManager = new QNetworkAccessManager(this);
}


void Synchronizer::checkConnection(QUrl url) {
    m_networkManager->clearAccessCache();
    connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(connectionCheckFinished(QNetworkReply*)));
    m_networkManager->get(QNetworkRequest(url));
}


void Synchronizer::connectionCheckFinished(QNetworkReply* reply) {
    disconnect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(connectionCheckFinished(QNetworkReply*)));

    if (reply == 0) {
        emit connectionFailed();
        synchronizationError(QNetworkReply::UnknownNetworkError, ConnectionCheck);
        return;
    }

    QNetworkReply::NetworkError error = reply->error();

    switch (error) {
    case QNetworkReply::NoError:
    case QNetworkReply::ContentNotFoundError:
        emit connectionReady();
        break;
    default:
        synchronizationError(error, ConnectionCheck);
        emit connectionFailed();
    }

    reply->deleteLater();
}


void Synchronizer::duplicateDownloadFinished(QNetworkReply* reply) {
    if (reply == 0) {
        synchronizationError(QNetworkReply::UnknownNetworkError, InspectRemoteFiles);
        return;
    }

    QNetworkReply::NetworkError error = reply->error();

    if (error == QNetworkReply::NoError) {
        inspectDuplicate(reply);
        m_duplicatesFileList.pop_front();
        downloadDuplicates();
    } else {
        synchronizationError(error, InspectRemoteFiles);
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
        synchronizationError(QNetworkReply::UnknownNetworkError, ExchangeFiles);
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
        synchronizationError(error, ExchangeFiles);
        return;
    }

    reply->deleteLater();
}


void Synchronizer::fileListSent(QNetworkReply* reply) {
    disconnect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileListSent(QNetworkReply*)));

    if (reply == 0) {
        synchronizationError(QNetworkReply::UnknownNetworkError, ExchangeFiles);
        return;
    }

    QNetworkReply::NetworkError error = reply->error();

    if (error == QNetworkReply::NoError) {
    } else {
        synchronizationError(error, SendFileList);
        return;
    }

    reply->deleteLater();
    synchronize(Finish);
}


void Synchronizer::getRemoteFileList(QNetworkReply *reply) {
    disconnect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getRemoteFileList(QNetworkReply*)));

    if (reply == 0) {
        synchronizationError(QNetworkReply::UnknownNetworkError, GetRemoteFileList);
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
        synchronizationError(error, GetRemoteFileList);
        reply->deleteLater();
        return;
    }

    reply->deleteLater();
    synchronize(InspectRemoteFiles);
}


void Synchronizer::inspectDuplicate(QNetworkReply* reply) {
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
            QDateTime remoteDate = reply->header(QNetworkRequest::LastModifiedHeader).toDateTime();
            QDateTime localDate = QFileInfo(file).lastModified();
            if (localDate > remoteDate)
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
    QSettings settings;
    QUrl url;

    url.setHost(settings.value("library/remote/address").toString());
    url.setPassword(settings.value("library/remote/password").toString());
    url.setPath(settings.value("library/remote/path").toString() + "/" + fileName);
    url.setPort(settings.value("library/remote/port").toInt());
    url.setScheme(settings.value("library/remote/protocol").toString());
    url.setUserName(settings.value("library/remote/userName").toString());

    return url;
}


void Synchronizer::synchronizationError(QNetworkReply::NetworkError error, SynchronizationStage stage) {
    if (error == QNetworkReply::UnknownNetworkError) {
        qDebug() << "Unknown error";
        return;
    }

    if (stage == ConnectionCheck)
        qDebug() << "Error in connection check:" << error;
    else if (stage == GetRemoteFileList)
        qDebug() << "Error while getting remote file list:" << error;
    else if (stage == InspectRemoteFiles)
        qDebug() << "Error while inspecting remote files:" << error;
    else if (stage == ExchangeFiles)
        qDebug() << "Error while exchanging files:" << error;
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
    return QString("%1%2%3").arg(m_localLibraryPath, QDir::separator(), fileName);
}
