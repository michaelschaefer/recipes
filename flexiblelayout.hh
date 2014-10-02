#ifndef FLEXIBLELAYOUT_HH
#define FLEXIBLELAYOUT_HH


#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QList>
#include <QWidget>
#include "widgetinterface.hh"


class FlexibleLayout : public QWidget {

    Q_OBJECT


signals:

    void changed();


public:

    FlexibleLayout(QWidget* parent=0);

    void addWidget(WidgetInterface* widget);
    QLayout* layout();
    QList<WidgetInterface*> widgets();


public slots:

    void down(WidgetInterface* widget);
    void remove(WidgetInterface* widget);
    void up(WidgetInterface* widget);


private slots:

    void triggerChanged();


private:    


    QGridLayout* m_layout;
    QLabel* m_empty;
    QList<WidgetInterface*> m_widgets;


};

#endif // FLEXIBLELAYOUT_HH
