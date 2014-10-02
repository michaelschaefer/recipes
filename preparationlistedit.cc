#include <QFont>
#include "preparationstepdialog.hh"
#include "preparationstepedit.hh"
#include "preparationlistedit.hh"


PreparationListEdit::PreparationListEdit(QWidget* parent)
    : QWidget(parent)
{
    QFont boldFont;
    boldFont.setBold(true);

    m_headline = new QLabel(trUtf8("Preparation"), this);
    m_headline->setFont(boldFont);

    m_flexibleLayout = new FlexibleLayout();
    connect(m_flexibleLayout, SIGNAL(changed()), this, SLOT(triggerChanged()));

    m_layout = new QVBoxLayout();
    m_layout->addWidget(m_headline);
    m_layout->addLayout(m_flexibleLayout->layout());
    m_layout->addStretch(100);
    setLayout(m_layout);
}


void PreparationListEdit::addPreparationStep() {
    PreparationStepDialog dialog(this);
    dialog.show();
    if (dialog.exec() == QDialog::Accepted) {
        addPreparationStep(dialog.text());
    }
}


void PreparationListEdit::addPreparationStep(QString text) {
    PreparationStepEdit* edit = new PreparationStepEdit(this);
    edit->setText(text);
    m_flexibleLayout->addWidget(edit);
    triggerChanged();
}


PreparationListEdit::DataType PreparationListEdit::data() {
    PreparationListEdit::DataType data;

    QList<WidgetInterface*> list = m_flexibleLayout->widgets();
    for (QList<WidgetInterface*>::Iterator it = list.begin(); it != list.end(); ++it) {
        data.append((*it)->data());
    }

    return data;
}


void PreparationListEdit::triggerChanged() {
    emit changed();
}
