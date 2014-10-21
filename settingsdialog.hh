#ifndef SETTINGSDIALOG_HH
#define SETTINGSDIALOG_HH


#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QTabWidget>
#include <QVBoxLayout>
#include "settingstablibrary.hh"


class SettingsDialog : public QDialog {

    Q_OBJECT


signals:

    void libraryPathChanged();


public:

    SettingsDialog(QWidget* parent = 0);


private slots:

    void save();


private:    

    void fill();
    void setupDialogButtons();


    QDialogButtonBox* m_buttonBox;
    QSettings m_settings;
    QTabWidget* m_tabWidget;
    QVBoxLayout* m_layout;
    SettingsTabLibrary* m_tabLibrary;


};

#endif // SETTINGSDIALOG_HH
