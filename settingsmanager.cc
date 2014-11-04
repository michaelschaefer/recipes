#include "settingsmanager.hh"

#include <QDir>


SettingsManager::SettingsManager() {
    m_settings = new QSettings();
    readFormatSettings();
    readLibrarySettings();
}


SettingsManager *SettingsManager::instance() {
    static SettingsManager instance;
    return &instance;
}


SettingsManager::FormatSettings SettingsManager::formatSettings() {
    return m_formatSettings;
}


SettingsManager::LibrarySettings SettingsManager::librarySettings() {
    return m_librarySettings;
}


void SettingsManager::readFormatSettings() {
    m_settings->beginGroup("format");
    m_settings->beginGroup("font");
    m_formatSettings.fontDefault = m_settings->value("default", true).toBool();
    m_formatSettings.fontFamilyName = m_settings->value("familyName", QString()).toString();
    m_formatSettings.fontUser = m_settings->value("user", false).toBool();
    m_settings->endGroup();
    m_settings->beginGroup("paragraphTitles");
    m_formatSettings.paragraphDefault = m_settings->value("default", true).toBool();
    m_formatSettings.paragraphEmptyHeadline = m_settings->value("emptyHeadline").toString();
    m_formatSettings.paragraphIngredients = m_settings->value("ingredients").toString();
    m_formatSettings.paragraphPreparation = m_settings->value("preparation").toString();
    m_formatSettings.paragraphUser = m_settings->value("user", false).toBool();
    m_settings->endGroup();
    m_settings->endGroup();
}


void SettingsManager::readLibrarySettings() {
    m_settings->beginGroup("library");
    m_settings->beginGroup("local");
    m_librarySettings.localPath = m_settings->value("path").toString();
    m_librarySettings.syncOnQuit = m_settings->value("syncOnQuit", false).toBool();
    m_librarySettings.syncOnStart = m_settings->value("syncOnStart", false).toBool();
    m_settings->endGroup();
    m_settings->beginGroup("remote");
    m_librarySettings.remoteAddress = m_settings->value("address").toString();
    m_librarySettings.remotePassword = m_settings->value("password").toString();
    m_librarySettings.remotePath = m_settings->value("path", QDir::separator()).toString();
    m_librarySettings.remotePort = m_settings->value("port", 21).toInt();
    m_librarySettings.remoteProtocol = m_settings->value("protocol", "ftp").toString();
    m_librarySettings.remoteUserName = m_settings->value("userName", "anonymous").toString();
    m_librarySettings.useRemote = m_settings->value("enabled", false).toBool();
    m_settings->endGroup();
    m_settings->endGroup();
}


void SettingsManager::setFormatSettings(FormatSettings formatSettings) {
    m_settings->beginGroup("format");
    m_settings->beginGroup("font");
    bool oldFontDefault = m_settings->value("default").toBool();
    m_settings->setValue("default", formatSettings.fontDefault);
    QString oldFamilyName = m_settings->value("familyName").toString();
    m_settings->setValue("familyName", formatSettings.fontFamilyName);
    m_settings->setValue("user", formatSettings.fontUser);
    m_settings->endGroup();
    m_settings->beginGroup("paragraphTitles");
    bool oldParagraphDefault = m_settings->value("default").toBool();
    m_settings->setValue("default", formatSettings.paragraphDefault);
    QString oldEmptyHeadline = m_settings->value("emptyHeadline").toString();
    m_settings->setValue("emptyHeadline", formatSettings.paragraphEmptyHeadline);
    QString oldIngredients = m_settings->value("ingredients").toString();
    m_settings->setValue("ingredients", formatSettings.paragraphIngredients);
    QString oldPreparation = m_settings->value("preparation").toString();
    m_settings->setValue("preparation", formatSettings.paragraphPreparation);
    m_settings->setValue("user", formatSettings.paragraphUser);
    m_settings->endGroup();
    m_settings->endGroup();

    readFormatSettings();

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
}


void SettingsManager::setLibrarySettings(LibrarySettings librarySettings) {
    m_settings->beginGroup("library");
    m_settings->beginGroup("local");
    QString oldLibraryPath = m_settings->value("path").toString();
    m_settings->setValue("path", librarySettings.localPath);
    m_settings->setValue("syncOnQuit", librarySettings.syncOnQuit);
    m_settings->setValue("syncOnStart", librarySettings.syncOnStart);
    m_settings->endGroup();
    m_settings->beginGroup("remote");
    m_settings->setValue("enabled", librarySettings.useRemote);
    m_settings->setValue("address", librarySettings.remoteAddress);
    m_settings->setValue("password", librarySettings.remotePassword);
    m_settings->setValue("path", librarySettings.remotePath);
    m_settings->setValue("port", librarySettings.remotePort);
    m_settings->setValue("protocol", librarySettings.remoteProtocol);
    m_settings->setValue("userName", librarySettings.remoteUserName);
    m_settings->endGroup();
    m_settings->endGroup();

    readLibrarySettings();

    if (oldLibraryPath != librarySettings.localPath)
        emit libraryPathChanged();
}
