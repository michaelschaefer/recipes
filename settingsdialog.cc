#include <QHBoxLayout>
#include <QPushButton>
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
}


void SettingsDialog::save() {
    accept();
}


void SettingsDialog::setupDialogButtons() {
    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->addButton(trUtf8("Apply"), QDialogButtonBox::AcceptRole);
    m_buttonBox->addButton(trUtf8("Cancel"), QDialogButtonBox::RejectRole);
}
