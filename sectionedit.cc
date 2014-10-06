#include <QInputDialog>
#include "sectionedit.hh"


SectionEdit::SectionEdit(QWidget* parent)
    : WidgetInterface(parent)
{   
    m_data["type"] = "section";
    m_data["title"] = "";
}


void SectionEdit::edit() {
    bool okay;
    QString title = trUtf8("Edit section name");
    QString label = trUtf8("Input new name of section:");
    QString text = m_text->text();

    text = QInputDialog::getText(this, title, label, QLineEdit::Normal, text, &okay);
    if (okay == true) {
        setText(text);
    }
}


void SectionEdit::setText(QString text) {
    if (text != m_text->text()) {
        m_text->setText(text);
        m_data["title"] = text;
        emit changed();
    }
}


QString SectionEdit::text() {
    return m_text->text();
}
