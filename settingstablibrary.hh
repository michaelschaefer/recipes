#ifndef SETTINGSTABLIBRARY_HH
#define SETTINGSTABLIBRARY_HH


#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>
#include "ftpmanager.hh"


class SettingsTabLibrary : public QWidget {

    Q_OBJECT


public:

    SettingsTabLibrary(QWidget* parent = 0);


private slots:

    void checkConnection();
    void choosePath();
    void connectionChecked(bool successful);
    void toggleRemote();


private:

    void setupGroupBoxLocal();
    void setupGroupBoxRemote();


    FtpManager* m_ftpManager;
    QFormLayout* m_layout;
    QFormLayout* m_layoutRemote;
    QGroupBox* m_groupBoxLocal;
    QGroupBox* m_groupBoxRemote;
    QHBoxLayout* m_layoutLocal;
    QLineEdit* m_editLocalPath;
    QLineEdit* m_editRemotePassword;
    QLineEdit* m_editRemoteServer;
    QLineEdit* m_editRemotePath;
    QLineEdit* m_editRemoteUserName;
    QPushButton* m_buttonLocalPath;
    QPushButton* m_buttonRemoteCheckConnection;
    QSpinBox* m_spinRemotePort;

};

#endif // SETTINGSTABLIBRARY_HH
