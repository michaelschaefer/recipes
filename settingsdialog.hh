#ifndef SETTINGSDIALOG_HH
#define SETTINGSDIALOG_HH


#include <QDialog>
#include <QDialogButtonBox>
#include "settingstabformat.hh"
#include "settingstablibrary.hh"


class SettingsDialog : public QDialog {

    Q_OBJECT


public:

    SettingsDialog(QWidget* parent = 0);


private slots:

    void save();


private:

    void fill();
    void setupDialogButtons();


    QDialogButtonBox* m_buttonBox;
    QTabWidget* m_tabWidget;
    QVBoxLayout* m_layout;
    SettingsManager* m_settingsManager;
    SettingsTabFormat* m_tabFormat;
    SettingsTabLibrary* m_tabLibrary;


};

#endif // SETTINGSDIALOG_HH
