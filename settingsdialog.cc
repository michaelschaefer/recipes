#include "settingsdialog.hh"


SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
    m_settingsManager = SettingsManager::instance();

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
    m_tabFormat->setSettings(m_settingsManager->formatSettings());
    m_tabLibrary->setSettings(m_settingsManager->librarySettings());
}


void SettingsDialog::save() {
    m_settingsManager->setFormatSettings(m_tabFormat->settings());
    m_settingsManager->setLibrarySettings(m_tabLibrary->settings());
    accept();
}


void SettingsDialog::setupDialogButtons() {
    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->addButton(trUtf8("Apply"), QDialogButtonBox::AcceptRole);
    m_buttonBox->addButton(trUtf8("Cancel"), QDialogButtonBox::RejectRole);
}
