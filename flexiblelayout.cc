#include "flexiblelayout.hh"


FlexibleLayout::FlexibleLayout(QWidget* parent) {
    m_layout = new QGridLayout(parent);
    m_empty = new QLabel(trUtf8("no entries yet"), this);
    m_layout->addWidget(m_empty, 0, 0);
}


void FlexibleLayout::addWidget(WidgetInterface* widget) {
    if (m_widgets.size() == 0) {
        m_layout->removeWidget(m_empty);
        delete m_empty;
        m_empty = 0;
    }

    m_layout->addWidget(widget, m_widgets.size(), 0);
    m_widgets.append(widget);

    connect(widget, SIGNAL(downTriggered(WidgetInterface*)), this, SLOT(down(WidgetInterface*)));
    connect(widget, SIGNAL(removeTriggered(WidgetInterface*)), this, SLOT(remove(WidgetInterface*)));
    connect(widget, SIGNAL(upTriggered(WidgetInterface*)), this, SLOT(up(WidgetInterface*)));
    connect(widget, SIGNAL(changed()), this, SLOT(triggerChanged()));
}


void FlexibleLayout::down(WidgetInterface* widget) {
    int pos = m_widgets.indexOf(widget);
    if (pos != -1 && pos < m_widgets.size() - 1) {
        QWidget* tmp = m_widgets[pos+1];
        m_widgets.move(pos+1, pos);
        m_layout->addWidget(tmp, pos, 0);
        m_layout->addWidget(widget, pos+1, 0);
        triggerChanged();
    }
}


QLayout* FlexibleLayout::layout() {
    return m_layout;
}


void FlexibleLayout::remove(WidgetInterface* widget) {    
    int index = m_layout->indexOf(widget);
    if (index != -1) {
        widget->setParent(0); // <- this revents "double free or corruption" errors
        m_layout->removeWidget(widget);
        m_widgets.removeAll(widget);
        triggerChanged();
    }        

    if (m_widgets.size() == 0) {
        m_empty = new QLabel(trUtf8("no entries yet"), this);
        m_layout->addWidget(m_empty, 0, 0);
    }
}


void FlexibleLayout::triggerChanged() {
    emit changed();
}


void FlexibleLayout::up(WidgetInterface* widget) {
    int pos = m_widgets.indexOf(widget);
    if (pos != -1 && pos > 0) {
        QWidget* tmp = m_widgets[pos-1];
        m_widgets.move(pos, pos-1);
        m_layout->addWidget(widget, pos-1, 0);
        m_layout->addWidget(tmp, pos, 0);
        triggerChanged();
    }
}


QList<WidgetInterface*> FlexibleLayout::widgets() {
    return m_widgets;
}
