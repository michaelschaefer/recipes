#include <QDebug>
#include "library.hh"
#include "searchfilterwidget.hh"


SearchFilterWidget::SearchFilterWidget(QWidget* parent) : QWidget(parent) {
    m_editHeadline = new QLineEdit(this);
    m_editHeadline->setPlaceholderText(trUtf8("Enter search query"));

    m_editIngredients = new QPlainTextEdit(this);    
    m_editIngredients->installEventFilter(this);

    m_layout = new QFormLayout();
    m_layout->setMargin(0);
    setLayout(m_layout);

    m_layout->addRow(trUtf8("Ingredients filter:"), m_editIngredients);
    m_layout->addRow(trUtf8("Headline filter:"), m_editHeadline);

    connect(m_editHeadline, SIGNAL(textChanged(QString)), this, SLOT(updateFilter()));
}


bool SearchFilterWidget::eventFilter(QObject* target, QEvent* event) {
    if (target == m_editIngredients && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);        
        if (keyEvent != 0 && (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)) {
            updateIngredientIdList();
            updateFilter();
            return QWidget::eventFilter(target, event);
        }
    }
    return QWidget::eventFilter(target, event);
}


QString SearchFilterWidget::headlineFilter() {
    return m_editHeadline->text();
}


QStringList SearchFilterWidget::ingredientFilter() {
    return m_editIngredients->toPlainText().split("\n");
}


void SearchFilterWidget::setFocus() {
    m_editHeadline->setFocus();
}


void SearchFilterWidget::updateFilter() {
    emit filterUpdated(headlineFilter(), m_ingredientIdList);
}


void SearchFilterWidget::updateIngredientIdList() {
    Library* library = Library::instance();
    QStringList ingredientNameList = m_editIngredients->toPlainText().split("\n");
    m_ingredientIdList.clear();
    foreach (const QString& ingredientName, ingredientNameList) {
        if (ingredientName.trimmed().isEmpty() == true)
            continue;
        QList<int> ingredientIdList = library->getIngredientIdList(ingredientName);
        if (ingredientIdList.isEmpty()) {
            m_ingredientIdList.clear();
            m_ingredientIdList.append(QList<int>());
            break;
        } else {
            m_ingredientIdList.append(ingredientIdList);
        }
    }
}
