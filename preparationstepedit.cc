#include "preparationstepdialog.hh"
#include "preparationstepedit.hh"


PreparationStepEdit::PreparationStepEdit(QWidget* parent)
    : WidgetInterface(parent)
{
    m_text->setWordWrap(true);
    m_data["type"] = "preparationStep";
    m_data["text"] = "";
}


void PreparationStepEdit::edit() {
    PreparationStepDialog dialog(this);
    dialog.setText(text());
    dialog.show();
    if (dialog.exec() == QDialog::Accepted) {
        setText(dialog.text());
    }
}


void PreparationStepEdit::setText(QString text) {
    if (text != m_text->text()) {
        m_text->setText(text);
        m_data["text"] = text;
        emit changed();
    }
}


QString PreparationStepEdit::text() {
    return m_text->text();
}
