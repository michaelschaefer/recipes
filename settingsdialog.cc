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
    m_tabFormat = new SettingsTabFormat(this);
    m_tabLibrary = new SettingsTabLibrary(this);

    // tab widget
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(m_tabFormat, trUtf8("Formatting"));
    m_tabWidget->addTab(m_tabLibrary, trUtf8("Library"));

    m_layout->addWidget(m_tabWidget);
    m_layout->addWidget(m_buttonBox, Qt::AlignRight);

    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(save()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    fill();
}


void SettingsDialog::fill() {
    SettingsTabLibrary::LibrarySettings librarySettings;
    SettingsTabFormat::FormatSettings formatSettings;

    m_settings.beginGroup("library");
    m_settings.beginGroup("local");
    librarySettings.localPath = m_settings.value("path").toString();
    librarySettings.syncOnQuit = m_settings.value("syncOnQuit", false).toBool();
    librarySettings.syncOnStart = m_settings.value("syncOnStart", false).toBool();
    m_settings.endGroup();
    m_settings.beginGroup("remote");
    librarySettings.remoteAddress = m_settings.value("address").toString();
    librarySettings.remotePassword = m_settings.value("password").toString();
    librarySettings.remotePath = m_settings.value("path", QDir::separator()).toString();
    librarySettings.remotePort = m_settings.value("port", 21).toInt();
    librarySettings.remoteProtocol = m_settings.value("protocol", "ftp").toString();
    librarySettings.remoteUserName = m_settings.value("userName", "anonymous").toString();    
    librarySettings.useRemote = m_settings.value("enabled", false).toBool();
    m_settings.endGroup();
    m_settings.endGroup();
    m_tabLibrary->setSettings(librarySettings);

    m_settings.beginGroup("format");
    m_settings.beginGroup("font");
    formatSettings.fontDefault = m_settings.value("default", true).toBool();
    formatSettings.fontFamilyName = m_settings.value("familyName", QString()).toString();
    formatSettings.fontUser = m_settings.value("user", false).toBool();
    m_settings.endGroup();
    m_settings.beginGroup("paragraphTitles");
    formatSettings.paragraphDefault = m_settings.value("default", true).toBool();
    formatSettings.paragraphEmptyHeadline = m_settings.value("emptyHeadline").toString();
    formatSettings.paragraphIngredients = m_settings.value("ingredients").toString();
    formatSettings.paragraphPreparation = m_settings.value("preparation").toString();
    formatSettings.paragraphUser = m_settings.value("user", false).toBool();
    m_settings.endGroup();
    m_settings.endGroup();
    m_tabFormat->setSettings(formatSettings);
}


void SettingsDialog::save() {
    SettingsTabLibrary::LibrarySettings librarySettings = m_tabLibrary->settings();
    SettingsTabFormat::FormatSettings formatSettings = m_tabFormat->settings();

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
    m_settings.setValue("protocol", librarySettings.remoteProtocol);
    m_settings.setValue("userName", librarySettings.remoteUserName);
    m_settings.endGroup();
    m_settings.endGroup();

    m_settings.beginGroup("format");
    m_settings.beginGroup("font");
    bool oldFontDefault = m_settings.value("default").toBool();
    m_settings.setValue("default", formatSettings.fontDefault);
    QString oldFamilyName = m_settings.value("familyName").toString();
    m_settings.setValue("familyName", formatSettings.fontFamilyName);
    m_settings.setValue("user", formatSettings.fontUser);
    m_settings.endGroup();
    m_settings.beginGroup("paragraphTitles");
    bool oldParagraphDefault = m_settings.value("default").toBool();
    m_settings.setValue("default", formatSettings.paragraphDefault);
    QString oldEmptyHeadline = m_settings.value("emptyHeadline").toString();
    m_settings.setValue("emptyHeadline", formatSettings.paragraphEmptyHeadline);
    QString oldIngredients = m_settings.value("ingredients").toString();
    m_settings.setValue("ingredients", formatSettings.paragraphIngredients);
    QString oldPreparation = m_settings.value("preparation").toString();
    m_settings.setValue("preparation", formatSettings.paragraphPreparation);
    m_settings.setValue("user", formatSettings.paragraphUser);
    m_settings.endGroup();
    m_settings.endGroup();

    if (oldLibraryPath != librarySettings.localPath)
        emit libraryPathChanged();

    if (oldFontDefault != formatSettings.fontDefault)
        emit fontChanged();
    else if (oldFamilyName != formatSettings.fontFamilyName)
        emit fontChanged();

    if (oldParagraphDefault != formatSettings.paragraphDefault)
        emit paragraphTitlesChanged();
    else if (oldEmptyHeadline != formatSettings.paragraphEmptyHeadline)
        emit paragraphTitlesChanged();
    else if (oldIngredients != formatSettings.paragraphIngredients)
        emit paragraphTitlesChanged();
    else if (oldPreparation != formatSettings.paragraphPreparation)
        emit paragraphTitlesChanged();

    accept();
}


void SettingsDialog::setupDialogButtons() {
    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->addButton(trUtf8("Apply"), QDialogButtonBox::AcceptRole);
    m_buttonBox->addButton(trUtf8("Cancel"), QDialogButtonBox::RejectRole);
}
