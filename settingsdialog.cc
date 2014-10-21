#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>
#include "mainwindow.hh"
#include "settingsdialog.hh"


SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
    setupDialogButtons();

    m_layout = new QVBoxLayout();
    setLayout(m_layout);

    // tabs
    m_tabLibrary = new SettingsTabLibrary(this);

    // tab widget
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(m_tabLibrary, trUtf8("Library"));    

    m_layout->addWidget(m_tabWidget);
    m_layout->addWidget(m_buttonBox, Qt::AlignRight);

    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(save()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    fill();
}


void SettingsDialog::fill() {
    SettingsTabLibrary::LibrarySettings librarySettings;

    m_settings.beginGroup("library");
    m_settings.beginGroup("local");
    librarySettings.localPath = m_settings.value("path", QString()).toString();
    librarySettings.syncOnQuit = m_settings.value("syncOnQuit", false).toBool();
    librarySettings.syncOnStart = m_settings.value("syncOnStart", false).toBool();
    m_settings.endGroup();
    m_settings.beginGroup("remote");
    librarySettings.remoteAddress = m_settings.value("address", QString()).toString();
    librarySettings.remotePassword = m_settings.value("password", QString()).toString();
    librarySettings.remotePath = m_settings.value("path", QDir::separator()).toString();
    librarySettings.remotePort = m_settings.value("port", 21).toInt();
    librarySettings.remoteUserName = m_settings.value("userName", "anonymous").toString();
    librarySettings.useRemote = m_settings.value("enabled", false).toBool();
    m_settings.endGroup();
    m_settings.endGroup();
    m_tabLibrary->setSettings(librarySettings);
}


void SettingsDialog::save() {    
    SettingsTabLibrary::LibrarySettings librarySettings = m_tabLibrary->settings();
    m_settings.beginGroup("library");
    m_settings.beginGroup("local");
    QString oldLibraryPath = m_settings.value("path").toString();    
    m_settings.setValue("path", librarySettings.localPath);
    m_settings.setValue("syncOnQuit", librarySettings.syncOnQuit);
    m_settings.setValue("syncOnStart", librarySettings.syncOnStart);
    m_settings.endGroup();
    m_settings.beginGroup("remote");
    m_settings.setValue("enabled", librarySettings.useRemote);
    m_settings.setValue("address", librarySettings.remoteAddress);
    m_settings.setValue("password", librarySettings.remotePassword);
    m_settings.setValue("path", librarySettings.remotePath);
    m_settings.setValue("port", librarySettings.remotePort);
    m_settings.setValue("userName", librarySettings.remoteUserName);
    m_settings.endGroup();
    m_settings.endGroup();

    if (oldLibraryPath != librarySettings.localPath)
        emit libraryPathChanged();
    accept();    
}


void SettingsDialog::setupDialogButtons() {
    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->addButton(trUtf8("Apply"), QDialogButtonBox::AcceptRole);
    m_buttonBox->addButton(trUtf8("Cancel"), QDialogButtonBox::RejectRole);
}
