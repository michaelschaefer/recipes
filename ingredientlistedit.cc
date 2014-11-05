#include <QInputDialog>
#include "ingredientdialog.hh"
#include "ingredientedit.hh"
#include "ingredientlistedit.hh"
#include "sectionedit.hh"
#include "settingsmanager.hh"


IngredientListEdit::IngredientListEdit(QWidget* parent)
    : QWidget(parent)
{
    m_boldFont.setBold(true);

    m_headline = new QLabel(this);
    m_headline->setFont(m_boldFont);
    updateHeadline();

    m_flexibleLayout = new FlexibleLayout();
    connect(m_flexibleLayout, SIGNAL(changed()), this, SLOT(triggerChanged()));


    m_layout = new QVBoxLayout();
    m_layout->addWidget(m_headline);
    m_layout->addLayout(m_flexibleLayout->layout());
    m_layout->addStretch(100);
    setLayout(m_layout);
}


void IngredientListEdit::addIngredient() {
    IngredientDialog dialog(this);
    dialog.show();
    if (dialog.exec() == QDialog::Accepted) {
        addIngredient(dialog.amount(), dialog.unit(), dialog.name());
    }
}


void IngredientListEdit::addIngredient(QString amount, QString unit, QString name) {
    IngredientEdit* edit = new IngredientEdit(this);
    edit->setAmount(amount);
    edit->setName(name);
    edit->setUnit(unit);
    m_flexibleLayout->addWidget(edit);
    triggerChanged();
}


void IngredientListEdit::addSection() {
    bool okay;
    QString label = trUtf8("Input name of new section:");
    QString title = trUtf8("Add section");

    QString sectionName = QInputDialog::getText(this, title, label, QLineEdit::Normal, "", &okay);
    if (okay == true) {
        addSection(sectionName);
    }
}


void IngredientListEdit::addSection(QString sectionName) {
    SectionEdit* edit = new SectionEdit();
    edit->setText(sectionName);
    edit->setFont(m_boldFont);
    m_flexibleLayout->addWidget(edit);
    triggerChanged();
}


QList<QMap<QString, QString> > IngredientListEdit::data() {
    QList<QMap<QString, QString> > data;
    QList<WidgetInterface*> list = m_flexibleLayout->widgets();
    for (QList<WidgetInterface*>::Iterator it = list.begin(); it != list.end(); ++it) {
        data.append((*it)->data());
    }

    return data;
}


void IngredientListEdit::editServingCount() {
    bool okay;
    QString label = trUtf8("For how many servings do the ingredients suffice?");
    QString title = trUtf8("Edit serving count");

    QString servingCount = QInputDialog::getText(this, title, label, QLineEdit::Normal, m_servingCount, &okay);
    if (okay == true && m_servingCount != servingCount) {
        m_servingCount = servingCount;
        updateHeadline();
        triggerChanged();
    }
}


QString IngredientListEdit::servingCount() {
    return m_servingCount;
}


void IngredientListEdit::triggerChanged() {
    emit changed();
}


void IngredientListEdit::updateHeadline() {
    SettingsManager::FormatSettings formatSettings = SettingsManager::instance()->formatSettings();
    QString headline = trUtf8("Ingredients");
    if (!formatSettings.paragraphDefault)
        headline = formatSettings.paragraphIngredients;
    if (m_servingCount.isEmpty() == false) {
        headline += " " + m_servingCount;
    }
    m_headline->setText(headline);
}

