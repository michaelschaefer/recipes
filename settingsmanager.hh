#ifndef SETTINGSMANAGER_HH
#define SETTINGSMANAGER_HH


#include <QObject>
#include <QSettings>


class SettingsManager : public QObject {

    Q_OBJECT


signals:

    void fontChanged();
    void libraryPathChanged();
    void paragraphTitlesChanged();


public:

    struct FormatSettings {
        bool fontDefault;
        QString fontFamilyName;
        bool fontUser;
        bool paragraphDefault;
        QString paragraphEmptyHeadline;
        QString paragraphIngredients;
        QString paragraphPreparation;
        bool paragraphUser;
    };

    struct LibrarySettings {
        QString localPath;
        bool syncOnStart;
        bool syncOnQuit;
        bool useRemote;
        QString remoteProtocol;
        QString remoteAddress;
        QString remotePath;
        int remotePort;
        QString remoteUserName;
        QString remotePassword;
    };


    static SettingsManager* instance();

    FormatSettings formatSettings();
    LibrarySettings librarySettings();
    void setFormatSettings(FormatSettings formatSettings);
    void setLibrarySettings(LibrarySettings librarySettings);


private:

    SettingsManager();
    SettingsManager(SettingsManager const&);
    void operator=(SettingsManager const&);

    void readFormatSettings();
    void readLibrarySettings();


    FormatSettings m_formatSettings;
    LibrarySettings m_librarySettings;
    QSettings* m_settings;

};

#endif // SETTINGSMANAGER_HH
