#include <QDebug>
#include <QCheckBox>
#include <QGroupBox>
#include "library.hh"
#include "searchfilterwidget.hh"


SearchFilterWidget::SearchFilterWidget(QWidget* parent) : QWidget(parent) {
    setupPopupMenu();

    m_filterSettings.caseSensitivity = Qt::CaseInsensitive;

    m_editHeadline = new QLineEdit(this);
    m_editHeadline->setPlaceholderText(trUtf8("Enter search query"));

    m_layoutIngredients = new QVBoxLayout();    

    m_listWidgetIngredients = new QListWidget(this);
    m_listWidgetIngredients->installEventFilter(this);
    m_listWidgetIngredients->setSortingEnabled(true);

    m_lineEditIngredients = new QLineEdit(this);
    m_lineEditIngredients->setPlaceholderText(trUtf8("Press enter to add ingredient"));

    m_layoutIngredients->addWidget(m_lineEditIngredients);
    m_layoutIngredients->addWidget(m_listWidgetIngredients);

    m_layoutInput = new QFormLayout();
    m_layoutInput->setMargin(0);
    m_layoutInput->addRow(trUtf8("Ingredients filter:"), m_layoutIngredients);
    m_layoutInput->addRow(trUtf8("Headline filter:"), m_editHeadline);

    m_groupBoxSearchSettings = new QGroupBox(trUtf8("Search settings"), this);
    m_groupBoxSearchSettings->setLayout(new QVBoxLayout());
    m_checkBoxCaseSensitive = new QCheckBox(trUtf8("Case sensitive"), m_groupBoxSearchSettings);
    m_groupBoxSearchSettings->layout()->addWidget(m_checkBoxCaseSensitive);

    m_layout = new QVBoxLayout();
    m_layout->setMargin(0);
    m_layout->addWidget(m_groupBoxSearchSettings);
    m_layout->addLayout(m_layoutInput);
    setLayout(m_layout);

    connect(m_editHeadline, SIGNAL(textChanged(QString)), this, SLOT(updateFilter()));
    connect(m_lineEditIngredients, SIGNAL(returnPressed()), this, SLOT(insertIngredient()));
    connect(m_listWidgetIngredients, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editCurrentListItem()));
    connect(m_checkBoxCaseSensitive, SIGNAL(toggled(bool)), this, SLOT(updateFilterSettings()));
}


void SearchFilterWidget::editCurrentListItem() {
    QModelIndex index = m_listWidgetIngredients->selectionModel()->currentIndex();
    if (index.isValid() == true) {
        QString ingredientName = index.data().toString();
        m_lineEditIngredients->setText(ingredientName);
        m_lineEditIngredients->setFocus();
        m_ingredientNameList.removeAll(index.data().toString());
        m_listWidgetIngredients->model()->removeRow(index.row());
        updateIngredientIdList();
        updateFilter();
    }
}


bool SearchFilterWidget::eventFilter(QObject* target, QEvent* event) {        
    if (target == m_listWidgetIngredients && event->type() == QEvent::ContextMenu) {
        QContextMenuEvent* contextMenuEvent = static_cast<QContextMenuEvent*>(event);
        if (contextMenuEvent != 0) {
            QPoint globalPos = contextMenuEvent->globalPos();
            QPoint localPos = globalPos - m_listWidgetIngredients->mapToGlobal(QPoint(0,0));
            QListWidgetItem* item = m_listWidgetIngredients->itemAt(localPos);
            if (item != 0)
                m_popupMenu->exec(globalPos);
        }
    }

    if (target == m_listWidgetIngredients && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent != 0) {
            int key = keyEvent->key();
            if (key == Qt::Key_Delete)
                removeCurrentListItem();
            else if (key == Qt::Key_Return || key == Qt::Key_Enter)
                editCurrentListItem();
        }
    }

    return QWidget::eventFilter(target, event);
}


void SearchFilterWidget::insertIngredient() {
    QString ingredientName = m_lineEditIngredients->text();
    m_ingredientNameList.append(ingredientName);
    m_listWidgetIngredients->addItem(ingredientName);
    m_lineEditIngredients->clear();
    updateIngredientIdList();
    updateFilter();
}


void SearchFilterWidget::removeCurrentListItem() {
    QModelIndex index = m_listWidgetIngredients->selectionModel()->currentIndex();
    if (index.isValid() == true) {
        m_ingredientNameList.removeAll(index.data().toString());
        m_listWidgetIngredients->model()->removeRow(index.row());
        updateIngredientIdList();
        updateFilter();
    }
}


void SearchFilterWidget::setupPopupMenu() {
    m_popupMenu = new QMenu(this);

    m_actionEdit = new QAction(trUtf8("Edit"), m_popupMenu);
    m_actionRemove = new QAction(trUtf8("Remove"), m_popupMenu);

    m_popupMenu->addAction(m_actionEdit);
    m_popupMenu->addAction(m_actionRemove);

    connect(m_actionEdit, SIGNAL(triggered()), this, SLOT(editCurrentListItem()));
    connect(m_actionRemove, SIGNAL(triggered()), this, SLOT(removeCurrentListItem()));
}


void SearchFilterWidget::setFocus() {
    m_editHeadline->setFocus();
}


void SearchFilterWidget::updateFilter() {
    emit filterUpdated(m_editHeadline->text(), m_ingredientIdList);
}


void SearchFilterWidget::updateFilterSettings() {
    if (m_checkBoxCaseSensitive->isChecked() == true)
        m_filterSettings.caseSensitivity = Qt::CaseSensitive;
    else
        m_filterSettings.caseSensitivity = Qt::CaseInsensitive;

    updateIngredientIdList();
    emit filterSettingsUpdated(m_filterSettings);
    updateFilter();
}


void SearchFilterWidget::updateIngredientIdList() {
    Library* library = Library::instance();
    m_ingredientIdList.clear();
    foreach (const QString& ingredientName, m_ingredientNameList) {
        if (ingredientName.trimmed().isEmpty() == true)
            continue;
        QList<int> ingredientIdList = library->getIngredientIdList(ingredientName, m_filterSettings.caseSensitivity);
        if (ingredientIdList.isEmpty()) {
            m_ingredientIdList.clear();
            m_ingredientIdList.append(QList<int>());
            break;
        } else {
            m_ingredientIdList.append(ingredientIdList);
        }
    }
}
