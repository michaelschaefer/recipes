#ifndef WIDGETINTERFACE_HH
#define WIDGETINTERFACE_HH


#include <QAction>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMap>
#include <QMenu>
#include <QMouseEvent>
#include <QPushButton>
#include <QWidget>


class WidgetInterface : public QWidget {

    Q_OBJECT    


signals:

    void changed();
    void downTriggered(WidgetInterface*);
    void removeTriggered(WidgetInterface*);
    void upTriggered(WidgetInterface*);


public:   

    typedef QMap<QString, QString> DataType;

    WidgetInterface(QWidget* parent = 0);    

    WidgetInterface::DataType data();


private slots:

    virtual void edit() = 0;
    void triggerDown();
    void triggerRemove();
    void triggerUp();


protected:   

    QAction* m_actionEdit;
    QAction* m_actionMoveDown;
    QAction* m_actionMoveUp;
    QAction* m_actionRemove;
    QHBoxLayout* m_layout;    
    QLabel* m_text;
    WidgetInterface::DataType m_data;
    QMenu* m_popupMenu;   


private:

    void mouseReleaseEvent(QMouseEvent* event);
    void setupPopupMenu();


};

#endif // WIDGETINTERFACE_HH
