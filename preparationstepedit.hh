#ifndef PREPARATIONSTEPEDIT_HH
#define PREPARATIONSTEPEDIT_HH


#include "widgetinterface.hh"


class PreparationStepEdit : public WidgetInterface {

    Q_OBJECT


public:

    PreparationStepEdit(QWidget* parent = 0);

    void edit();
    void setText(QString text);
    QString text();

};

#endif // PREPARATIONSTEPEDIT_HH
