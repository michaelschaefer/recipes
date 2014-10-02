#include "ingredientdialog.hh"


IngredientDialog::IngredientDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(trUtf8("Add Ingredient"));

    m_amountEdit = new QLineEdit(this);
    m_amountEdit->setFocus();
    m_unitEdit = new QLineEdit(this);
    m_nameEdit = new QLineEdit(this);

    m_formLayout = new QFormLayout();
    m_formLayout->addRow(trUtf8("Amount:"), m_amountEdit);
    m_formLayout->addRow(trUtf8("Unit:"), m_unitEdit);
    m_formLayout->addRow(trUtf8("Name:"), m_nameEdit);

    m_okay = new QPushButton(trUtf8("Okay"), this);
    connect(m_okay, SIGNAL(clicked()), this, SLOT(accept()));

    m_cancel = new QPushButton(trUtf8("Cancel"), this);
    connect(m_cancel, SIGNAL(clicked()), this, SLOT(reject()));

    m_layout = new QGridLayout();
    m_layout->addLayout(m_formLayout, 0, 0, 1, 2);
    m_layout->addWidget(m_okay, 1, 0);
    m_layout->addWidget(m_cancel, 1, 1);
    setLayout(m_layout);
}


QString IngredientDialog::amount() {
    return m_amountEdit->text();
}


QString IngredientDialog::name() {
    return m_nameEdit->text();
}


void IngredientDialog::setAmount(QString amount) {
    m_amountEdit->setText(amount);
}


void IngredientDialog::setName(QString name) {
    m_nameEdit->setText(name);
}


void IngredientDialog::setUnit(QString unit) {
    m_unitEdit->setText(unit);
}


QString IngredientDialog::unit() {
    return m_unitEdit->text();
}

