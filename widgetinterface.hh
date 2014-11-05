#ifndef WIDGETINTERFACE_HH
#define WIDGETINTERFACE_HH


#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>


class WidgetInterface : public QWidget {

    Q_OBJECT    


signals:

    void changed();
    void downTriggered(WidgetInterface*);
    void removeTriggered(WidgetInterface*);
    void upTriggered(WidgetInterface*);


public:       

    WidgetInterface(QWidget* parent = 0);    

    QMap<QString, QString> data();


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
    QMap<QString, QString> m_data;
    QMenu* m_popupMenu;   


private:

    void mouseReleaseEvent(QMouseEvent* event);
    void setupPopupMenu();


};

#endif // WIDGETINTERFACE_HH
