#include "settingstabformat.hh"


SettingsTabFormat::SettingsTabFormat(QWidget* parent) : QWidget(parent) {
    setupGroupBoxFont();
    setupGroupBoxParagraphTitles();

    m_layout = new QVBoxLayout();
    m_layout->addWidget(m_groupBoxFont);
    m_layout->addWidget(m_groupBoxParagraphTitles);
    m_layout->addStretch();
    setLayout(m_layout);

    connect(m_radioButtonUserFont, SIGNAL(toggled(bool)), m_comboBoxFont, SLOT(setEnabled(bool)));
    connect(m_radioButtonUserParagraph, SIGNAL(toggled(bool)), m_editParagraphTitleEmptyHeadline, SLOT(setEnabled(bool)));
    connect(m_radioButtonUserParagraph, SIGNAL(toggled(bool)), m_editParagraphTitleIngredients, SLOT(setEnabled(bool)));
    connect(m_radioButtonUserParagraph, SIGNAL(toggled(bool)), m_editParagraphTitlePreparation, SLOT(setEnabled(bool)));
    connect(m_radioButtonUserParagraph, SIGNAL(clicked()), m_editParagraphTitleEmptyHeadline, SLOT(setFocus()));
}


void SettingsTabFormat::setSettings(FormatSettings formatSettings) {
    m_comboBoxFont->setFont(QFont(formatSettings.fontFamilyName));
    m_radioButtonDefaultFont->setChecked(formatSettings.fontDefault);
    m_radioButtonUserFont->setChecked(formatSettings.fontUser);

    m_editParagraphTitleEmptyHeadline->setText(formatSettings.paragraphEmptyHeadline);
    m_editParagraphTitleIngredients->setText(formatSettings.paragraphIngredients);
    m_editParagraphTitlePreparation->setText(formatSettings.paragraphPreparation);
    m_radioButtonDefaultParagraph->setChecked(formatSettings.paragraphDefault);
    m_radioButtonUserParagraph->setChecked(formatSettings.paragraphUser);
}


SettingsTabFormat::FormatSettings SettingsTabFormat::settings() {
    FormatSettings formatSettings;

    formatSettings.fontDefault = m_radioButtonDefaultFont->isChecked();
    formatSettings.fontFamilyName = m_comboBoxFont->currentFont().family();
    formatSettings.fontUser = m_radioButtonUserFont->isChecked();
    formatSettings.paragraphDefault = m_radioButtonDefaultParagraph->isChecked();
    formatSettings.paragraphEmptyHeadline = m_editParagraphTitleEmptyHeadline->text();
    formatSettings.paragraphIngredients = m_editParagraphTitleIngredients->text();
    formatSettings.paragraphPreparation = m_editParagraphTitlePreparation->text();
    formatSettings.paragraphUser = m_radioButtonUserParagraph->isChecked();

    return formatSettings;
}


void SettingsTabFormat::setupGroupBoxFont() {
    m_groupBoxFont = new QGroupBox(trUtf8("Font"), this);

    m_radioButtonDefaultFont = new QRadioButton(trUtf8("use default font (Gentium)"), this);
    m_radioButtonDefaultFont->setChecked(true);
    m_radioButtonUserFont = new QRadioButton(trUtf8("use specified font"), this);
    m_radioButtonDefaultFont->setChecked(false);

    m_comboBoxFont = new QFontComboBox(this);
    m_comboBoxFont->setCurrentIndex(0);
    m_comboBoxFont->setEnabled(false);

    m_layoutFormat = new QVBoxLayout();
    m_layoutFormat->addWidget(m_radioButtonDefaultFont);
    m_layoutFormat->addWidget(m_radioButtonUserFont);
    m_layoutFormat->addWidget(m_comboBoxFont);
    m_groupBoxFont->setLayout(m_layoutFormat);
}


void SettingsTabFormat::setupGroupBoxParagraphTitles() {
    m_groupBoxParagraphTitles = new QGroupBox(trUtf8("Paragraph titles"));

    m_radioButtonDefaultParagraph = new QRadioButton(trUtf8("use default titles"), this);
    m_radioButtonDefaultParagraph->setChecked(true);
    m_radioButtonUserParagraph = new QRadioButton(trUtf8("use specified titles"), this);
    m_radioButtonUserParagraph->setChecked(false);

    m_editParagraphTitleEmptyHeadline = new QLineEdit(this);
    m_editParagraphTitleEmptyHeadline->setEnabled(false);
    m_editParagraphTitleIngredients = new QLineEdit(this);
    m_editParagraphTitleIngredients->setEnabled(false);
    m_editParagraphTitlePreparation = new QLineEdit(this);
    m_editParagraphTitlePreparation->setEnabled(false);

    m_layoutParagraphTitlesForm = new QFormLayout();
    m_layoutParagraphTitlesForm->addRow(trUtf8("Unnamed recipe:"), m_editParagraphTitleEmptyHeadline);
    m_layoutParagraphTitlesForm->addRow(trUtf8("Ingredients:"), m_editParagraphTitleIngredients);
    m_layoutParagraphTitlesForm->addRow(trUtf8("Preparation steps:"), m_editParagraphTitlePreparation);

    m_layoutParagraphTitles = new QVBoxLayout();
    m_layoutParagraphTitles->addWidget(m_radioButtonDefaultParagraph);
    m_layoutParagraphTitles->addWidget(m_radioButtonUserParagraph);
    m_layoutParagraphTitles->addLayout(m_layoutParagraphTitlesForm);
    m_groupBoxParagraphTitles->setLayout(m_layoutParagraphTitles);

}
