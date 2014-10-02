#include "ingredientdialog.hh"
#include "ingredientedit.hh"


IngredientEdit::IngredientEdit(QWidget* parent)
    : WidgetInterface(parent)
{
    m_data["type"] = "ingredient";
    m_data["amount"] = "";
    m_data["name"] = "";
    m_data["unit"] = "";
}


QString IngredientEdit::amount() {
    return m_amount;
}


void IngredientEdit::edit() {    
    IngredientDialog dialog(this);
    dialog.setAmount(amount());
    dialog.setName(name());
    dialog.setUnit(unit());
    dialog.show();

    if (dialog.exec() == QDialog::Accepted) {
        setAmount(dialog.amount());
        setName(dialog.name());
        setUnit(dialog.unit());
    }
}


QString IngredientEdit::name() {
    return m_name;
}


void IngredientEdit::setAmount(QString amount) {
    if (amount != m_amount) {
        m_amount = amount;
        m_data["amount"] = m_amount;
        updateText();
        emit changed();
    }
}


void IngredientEdit::setName(QString name) {
    if (name != m_name) {
        m_name = name;
        m_data["name"] = m_name;
        updateText();
        emit changed();
    }
}


void IngredientEdit::setUnit(QString unit) {
    if (unit != m_unit) {
        m_unit = unit;
        m_data["unit"] = m_unit;
        updateText();
        emit changed();
    }
}


QString IngredientEdit::unit() {
    return m_unit;
}


void IngredientEdit::updateText() {
    m_text->setText(amount() + unit() + "\t" + name());
}
