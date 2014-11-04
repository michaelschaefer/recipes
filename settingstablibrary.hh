#ifndef SETTINGSTABLIBRARY_HH
#define SETTINGSTABLIBRARY_HH


#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>
#include "synchronizer.hh"


class SettingsTabLibrary : public QWidget {

    Q_OBJECT


public:

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


    SettingsTabLibrary(QWidget* parent = 0);

    LibrarySettings settings();
    void setSettings(LibrarySettings settings);


private slots:

    void checkConnection();
    void choosePath();
    void connectionFailed();
    void connectionReady();    
    void toggleRemote();


private:

    void setupGroupBoxLocal();
    void setupGroupBoxRemote();


    QCheckBox* m_checkBoxSyncOnQuit;
    QComboBox* m_comboBoxProtocol;
    QCheckBox* m_checkBoxSyncOnStart;
    QFormLayout* m_layout;
    QFormLayout* m_layoutRemoteConnection;
    QGridLayout* m_layoutLocal;
    QGroupBox* m_groupBoxLocal;
    QGroupBox* m_groupBoxRemote;    
    QLineEdit* m_editLocalPath;
    QLineEdit* m_editRemotePassword;
    QLineEdit* m_editRemoteAddress;
    QLineEdit* m_editRemotePath;
    QLineEdit* m_editRemoteUserName;
    QPushButton* m_buttonLocalPath;
    QPushButton* m_buttonRemoteCheckConnection;
    QSpinBox* m_spinRemotePort;
    QVBoxLayout* m_layoutRemote;
    Synchronizer* m_synchronizer;

};

#endif // SETTINGSTABLIBRARY_HH
