#ifndef SECTIONEDIT_HH
#define SECTIONEDIT_HH


#include <QLabel>
#include "widgetinterface.hh"


class SectionEdit : public WidgetInterface {

    Q_OBJECT


public:

    SectionEdit(QWidget* parent = 0);

    void edit();    
    void setText(QString text);
    QString text();           

};

#endif // SECTIONEDIT_HH
