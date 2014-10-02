#ifndef INGREDIENTEDIT_HH
#define INGREDIENTEDIT_HH


#include <QLabel>
#include <QWidget>
#include "widgetinterface.hh"


class IngredientEdit : public WidgetInterface {

    Q_OBJECT


public:

    IngredientEdit(QWidget* parent = 0);

    QString amount();
    void edit();
    QString name();
    void setAmount(QString amount);
    void setName(QString name);
    void setUnit(QString unit);
    QString unit();


private:

    void updateText();


    QString m_amount;
    QString m_name;
    QString m_unit;


};

#endif // INGREDIENTEDIT_HH
