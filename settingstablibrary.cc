#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "settingstablibrary.hh"


SettingsTabLibrary::SettingsTabLibrary(QWidget *parent) : QWidget(parent) {
    m_ftpManager = FtpManager::instance();
    connect(m_ftpManager, SIGNAL(loginSuccess(bool)), this, SLOT(connectionChecked(bool)));

    setupGroupBoxLocal();
    setupGroupBoxRemote();

    m_layout = new QFormLayout();    
    m_layout->addRow(m_groupBoxLocal);
    m_layout->addRow(m_groupBoxRemote);
    setLayout(m_layout);

    connect(m_buttonLocalPath, SIGNAL(clicked()), this, SLOT(choosePath()));
    connect(m_buttonRemoteCheckConnection, SIGNAL(clicked()), this, SLOT(checkConnection()));
    connect(m_editLocalPath, SIGNAL(textChanged(QString)), this, SLOT(toggleRemote()));
}


void SettingsTabLibrary::checkConnection() {
    QUrl url;
    url.setHost(m_editRemoteServer->text());
    url.setPort(m_spinRemotePort->value());
    url.setUserName(m_editRemoteUserName->text());
    url.setPassword(m_editRemotePassword->text());
    url.setPath(m_editRemotePath->text());
    m_ftpManager->login(url);
}


void SettingsTabLibrary::choosePath() {
    QString caption = trUtf8("Choose path");
    QString pathName = QFileDialog::getExistingDirectory(this, caption, QDir::homePath());
    if (pathName.isEmpty() == false)
        m_editLocalPath->setText(pathName);
}


void SettingsTabLibrary::connectionChecked(bool successful) {
    QString title = trUtf8("Connection check");
    if (successful == true) {
        QString text = trUtf8("Connection successfully established.");
        QMessageBox::information(this, title, text);
        m_ftpManager->upload(QDir(m_editLocalPath->text()));
    } else {
        QString text = trUtf8("Connection failed.");
        QMessageBox::critical(this, title, text);
    }
}


void SettingsTabLibrary::setupGroupBoxLocal() {
    m_groupBoxLocal = new QGroupBox(trUtf8("Local"), this);
    m_groupBoxLocal->setEnabled(true);

    m_editLocalPath = new QLineEdit(m_groupBoxLocal);
    m_editLocalPath->setPlaceholderText(trUtf8("No path selected"));
    m_editLocalPath->setText("/home/mscha_08/Dokumente/rezepte");

    m_buttonLocalPath = new QPushButton(trUtf8("Choose path"), m_groupBoxLocal);

    m_layoutLocal = new QHBoxLayout();
    m_layoutLocal->addWidget(m_editLocalPath);
    m_layoutLocal->addWidget(m_buttonLocalPath);
    m_groupBoxLocal->setLayout(m_layoutLocal);
}


void SettingsTabLibrary::setupGroupBoxRemote() {
    m_groupBoxRemote = new QGroupBox(trUtf8("Remote"), this);
    m_groupBoxRemote->setCheckable(true);
    m_groupBoxRemote->setChecked(true);

    m_editRemotePassword = new QLineEdit(m_groupBoxRemote);
    m_editRemotePassword->setEchoMode(QLineEdit::Password);
    m_editRemotePassword->setText("eV89j5KvEQ");

    m_editRemotePath = new QLineEdit(m_groupBoxRemote);
    m_editRemotePath->setText("/rezepte");

    m_spinRemotePort = new QSpinBox(m_groupBoxRemote);
    m_spinRemotePort->setMinimum(1);
    m_spinRemotePort->setMaximum(65535);
    m_spinRemotePort->setValue(21);

    m_editRemoteServer = new QLineEdit(m_groupBoxRemote);
    m_editRemoteServer->setText("ftp.michael-schaefer.org");

    m_editRemoteUserName = new QLineEdit(m_groupBoxRemote);
    m_editRemoteUserName->setText("w005d352");

    m_buttonRemoteCheckConnection = new QPushButton(trUtf8("Check connection"), m_groupBoxRemote);

    m_layoutRemote = new QFormLayout();
    m_layoutRemote->addRow(trUtf8("Address:"), m_editRemoteServer);
    m_layoutRemote->addRow(trUtf8("Path:"), m_editRemotePath);
    m_layoutRemote->addRow(trUtf8("Port:"), m_spinRemotePort);
    m_layoutRemote->addRow(trUtf8("Username:"), m_editRemoteUserName);
    m_layoutRemote->addRow(trUtf8("Password:"), m_editRemotePassword);
    m_layoutRemote->addWidget(m_buttonRemoteCheckConnection);
    m_groupBoxRemote->setLayout(m_layoutRemote);

    toggleRemote();
}


void SettingsTabLibrary::toggleRemote() {
    m_groupBoxRemote->setEnabled(!m_editLocalPath->text().isEmpty());
}
