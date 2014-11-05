#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "settingstablibrary.hh"


SettingsTabLibrary::SettingsTabLibrary(QWidget *parent) : QWidget(parent) {
    m_synchronizer = new Synchronizer(this);
    connect(m_synchronizer, SIGNAL(connectionFailed(QString)), this, SLOT(connectionFailed(QString)));
    connect(m_synchronizer, SIGNAL(connectionReady()), this, SLOT(connectionReady()));

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
    url.setHost(m_editRemoteAddress->text());
    url.setPort(m_spinRemotePort->value());
    url.setUserName(m_editRemoteUserName->text());
    url.setPassword(m_editRemotePassword->text());
    url.setPath(m_editRemotePath->text() + "/files.txt");
    url.setScheme(m_comboBoxProtocol->currentText());

    m_synchronizer->checkConnection(url);
}


void SettingsTabLibrary::choosePath() {
    QString caption = trUtf8("Choose path");
    QString dir = m_editLocalPath->text();
    if (dir.isEmpty() == true)
        dir = QDir::homePath();
    QString pathName = QFileDialog::getExistingDirectory(this, caption, dir);
    if (pathName.isEmpty() == false)
        m_editLocalPath->setText(pathName);
}


void SettingsTabLibrary::connectionFailed(QString error) {
    QString title = trUtf8("Check connection");
    QString text = trUtf8("Connection failed.");
    if (!error.isEmpty())
        text += " " + trUtf8("Reason:") + "\n\n" + error;
    QMessageBox::critical(this, title, text);
}


void SettingsTabLibrary::connectionReady() {
    QString title = trUtf8("Check connection");
    QString text = trUtf8("Connection successfully established.");
    QMessageBox::information(this, title, text);    
}


void SettingsTabLibrary::setSettings(SettingsManager::LibrarySettings settings) {
    m_editLocalPath->setText(settings.localPath);
    m_comboBoxProtocol->setEditText(settings.remoteProtocol);
    m_editRemoteAddress->setText(settings.remoteAddress);
    m_editRemotePassword->setText(settings.remotePassword);
    m_editRemotePath->setText(settings.remotePath);
    m_spinRemotePort->setValue(settings.remotePort);
    m_editRemoteUserName->setText(settings.remoteUserName);
    m_checkBoxSyncOnQuit->setChecked(settings.syncOnQuit);
    m_checkBoxSyncOnStart->setChecked(settings.syncOnStart);
    m_groupBoxRemote->setChecked(settings.useRemote);

    if (settings.localPath.isEmpty() == true) {
        m_checkBoxSyncOnQuit->setEnabled(false);
        m_checkBoxSyncOnStart->setEnabled(false);
    }
}


SettingsManager::LibrarySettings SettingsTabLibrary::settings() {
    SettingsManager::LibrarySettings librarySettings;

    librarySettings.localPath = m_editLocalPath->text();
    librarySettings.remoteProtocol = m_comboBoxProtocol->currentText();
    librarySettings.remoteAddress = m_editRemoteAddress->text();
    librarySettings.remotePassword = m_editRemotePassword->text();
    librarySettings.remotePath = m_editRemotePath->text();
    librarySettings.remotePort = m_spinRemotePort->value();
    librarySettings.remoteUserName = m_editRemoteUserName->text();
    librarySettings.syncOnQuit = m_checkBoxSyncOnQuit->isChecked();
    librarySettings.syncOnStart = m_checkBoxSyncOnStart->isChecked();
    librarySettings.useRemote = m_groupBoxRemote->isChecked();

    return librarySettings;
}


void SettingsTabLibrary::setupGroupBoxLocal() {
    m_groupBoxLocal = new QGroupBox(trUtf8("Local"), this);
    m_groupBoxLocal->setEnabled(true);

    m_editLocalPath = new QLineEdit(m_groupBoxLocal);
    m_editLocalPath->setPlaceholderText(trUtf8("No path selected"));

    m_buttonLocalPath = new QPushButton(trUtf8("Choose path"), m_groupBoxLocal);

    m_checkBoxSyncOnStart = new QCheckBox(trUtf8("Synchronize on start"), m_groupBoxLocal);
    m_checkBoxSyncOnQuit = new QCheckBox(trUtf8("Synchronize on quit"), m_groupBoxLocal);

    m_layoutLocal = new QGridLayout();
    m_layoutLocal->addWidget(m_editLocalPath, 0, 0);
    m_layoutLocal->addWidget(m_buttonLocalPath, 0, 1);
    m_layoutLocal->addWidget(m_checkBoxSyncOnStart, 1, 0, 1, 2);
    m_layoutLocal->addWidget(m_checkBoxSyncOnQuit, 2, 0, 1, 2);
    m_groupBoxLocal->setLayout(m_layoutLocal);
}


void SettingsTabLibrary::setupGroupBoxRemote() {
    m_groupBoxRemote = new QGroupBox(trUtf8("Remote"), this);
    m_groupBoxRemote->setCheckable(true);
    m_groupBoxRemote->setChecked(false);

    m_comboBoxProtocol = new QComboBox(m_groupBoxRemote);
    m_comboBoxProtocol->setEditable(true);
    QStringList protocols;
    protocols << "ftp" << "http" << "https";
    m_comboBoxProtocol->addItems(protocols);

    m_editRemoteAddress = new QLineEdit(m_groupBoxRemote);
    m_editRemotePath = new QLineEdit(m_groupBoxRemote);

    m_spinRemotePort = new QSpinBox(m_groupBoxRemote);
    m_spinRemotePort->setMinimum(1);
    m_spinRemotePort->setMaximum(65535);

    m_editRemoteUserName = new QLineEdit(m_groupBoxRemote);

    m_editRemotePassword = new QLineEdit(m_groupBoxRemote);
    m_editRemotePassword->setEchoMode(QLineEdit::Password);            

    m_buttonRemoteCheckConnection = new QPushButton(trUtf8("Check connection"), m_groupBoxRemote);

    m_layoutRemoteConnection = new QFormLayout();
    m_layoutRemoteConnection->addRow(trUtf8("Protocol:"), m_comboBoxProtocol);
    m_layoutRemoteConnection->addRow(trUtf8("Address:"), m_editRemoteAddress);
    m_layoutRemoteConnection->addRow(trUtf8("Path:"), m_editRemotePath);
    m_layoutRemoteConnection->addRow(trUtf8("Port:"), m_spinRemotePort);
    m_layoutRemoteConnection->addRow(trUtf8("Username:"), m_editRemoteUserName);
    m_layoutRemoteConnection->addRow(trUtf8("Password:"), m_editRemotePassword);

    m_layoutRemote = new QVBoxLayout();
    m_layoutRemote->addLayout(m_layoutRemoteConnection);
    m_layoutRemote->addWidget(m_buttonRemoteCheckConnection, 0, Qt::AlignRight);

    m_groupBoxRemote->setLayout(m_layoutRemote);

    toggleRemote();
}


void SettingsTabLibrary::toggleRemote() {
    bool hasPath = !m_editLocalPath->text().isEmpty();
    m_checkBoxSyncOnQuit->setEnabled(hasPath);
    m_checkBoxSyncOnStart->setEnabled(hasPath);
    m_groupBoxRemote->setEnabled(hasPath);
}
