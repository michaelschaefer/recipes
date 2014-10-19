#include "settingstablibrary.hh"


SettingsTabLibrary::SettingsTabLibrary(QWidget *parent) : QWidget(parent) {
    m_layout = new QFormLayout();
    setLayout(m_layout);

    m_comboBoxLocation = new QComboBox(this);
    m_comboBoxLocation->addItem(trUtf8("local"));
    m_comboBoxLocation->addItem(trUtf8("remote"));

    m_groupBoxLocal = new QGroupBox(trUtf8("Local"), this);
    m_groupBoxLocal->setEnabled(true);
    m_groupBoxRemote = new QGroupBox(trUtf8("Remote"), this);
    m_groupBoxRemote->setEnabled(false);

    m_layout->addRow(trUtf8("Location:"), m_comboBoxLocation);
    m_layout->addRow(m_groupBoxLocal);
    m_layout->addRow(m_groupBoxRemote);

    connect(m_comboBoxLocation, SIGNAL(currentIndexChanged(int)), this, SLOT(toggleLocation(int)));
}


void SettingsTabLibrary::toggleLocation(int index) {
    if (index == 0) {
        m_groupBoxLocal->setEnabled(true);
        m_groupBoxRemote->setEnabled(false);
    } else {
        m_groupBoxLocal->setEnabled(false);
        m_groupBoxRemote->setEnabled(true);
    }
}
