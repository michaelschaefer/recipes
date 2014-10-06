#include <QMenu>
#include "widgetinterface.hh"


WidgetInterface::WidgetInterface(QWidget* parent)
    : QWidget(parent)    
{        
    setupPopupMenu();

    m_text = new QLabel(this);
    m_text->setFrameStyle(QFrame::Box);

    m_layout = new QHBoxLayout(this);    
    m_layout->addWidget(m_text);
    setLayout(m_layout);
}


WidgetInterface::DataType WidgetInterface::data() {
    return m_data;
}


void WidgetInterface::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) {        
        m_popupMenu->exec(event->globalPos());
    }
}


void WidgetInterface::setupPopupMenu() {
    m_popupMenu = new QMenu(this);

    m_actionEdit = new QAction(trUtf8("Edit"), m_popupMenu);
    m_actionMoveDown = new QAction(trUtf8("Move down"), m_popupMenu);
    m_actionMoveUp = new QAction(trUtf8("Move up"), m_popupMenu);
    m_actionRemove = new QAction(trUtf8("Remove"), m_popupMenu);

    connect(m_actionEdit, SIGNAL(triggered()), this, SLOT(edit()));
    connect(m_actionMoveDown, SIGNAL(triggered()), this, SLOT(triggerDown()));
    connect(m_actionMoveUp, SIGNAL(triggered()), this, SLOT(triggerUp()));
    connect(m_actionRemove, SIGNAL(triggered()), this, SLOT(triggerRemove()));

    m_popupMenu->addAction(m_actionEdit);
    m_popupMenu->addAction(m_actionRemove);
    m_popupMenu->addSeparator();

    m_popupMenu->addAction(m_actionMoveUp);  
    m_popupMenu->addAction(m_actionMoveDown);
}


void WidgetInterface::triggerDown() { emit downTriggered(this); }
void WidgetInterface::triggerRemove() { emit removeTriggered(this); }
void WidgetInterface::triggerUp() { emit upTriggered(this); }
