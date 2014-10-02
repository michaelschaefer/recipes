#ifndef INGREDIENTDIALOG_HH
#define INGREDIENTDIALOG_HH


#include <QDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>


class IngredientDialog : public QDialog {

    Q_OBJECT


public:

    IngredientDialog(QWidget* parent = 0);

    QString amount();
    QString name();
    void setAmount(QString amount);
    void setName(QString name);
    void setUnit(QString unit);
    QString unit();


private:

    QFormLayout* m_formLayout;
    QGridLayout* m_layout;
    QLineEdit* m_amountEdit;
    QLineEdit* m_nameEdit;
    QLineEdit* m_unitEdit;
    QPushButton* m_cancel;
    QPushButton* m_okay;


};

#endif // INGREDIENTDIALOG_HH
