#include "preparationstepdialog.hh"


PreparationStepDialog::PreparationStepDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(trUtf8("Add Preparation Step"));
    m_paragraphAllowed = false;

    m_edit = new QPlainTextEdit(this);
    m_edit->setFocus();

    m_okay = new QPushButton(trUtf8("Okay"), this);
    connect(m_okay, SIGNAL(clicked()), this, SLOT(accept()));

    m_cancel = new QPushButton(trUtf8("Cancel"), this);
    connect(m_cancel, SIGNAL(clicked()), this, SLOT(reject()));

    m_layout = new QGridLayout();
    m_layout->addWidget(m_edit, 0, 0, 1, 2);
    m_layout->addWidget(m_okay, 1, 0);
    m_layout->addWidget(m_cancel, 1, 1);
    setLayout(m_layout);
}


void PreparationStepDialog::setParagraphAllowed(bool allowed) {
    m_paragraphAllowed = allowed;
}


void PreparationStepDialog::setText(QString text) {
    if (m_paragraphAllowed == true) {
        text.replace("\n", "\n\n");
    } else {
        text.remove("\n");
    }

    m_edit->setPlainText(text);
}


QString PreparationStepDialog::text() {
    QString text = m_edit->toPlainText();
    if (m_paragraphAllowed == true) {
        text.replace("\n\n", "<br/>");
        text.remove("\n");
        text.replace("<br/>", "\n");
    } else {
        text.remove("\n");
    }

    return text;
}
