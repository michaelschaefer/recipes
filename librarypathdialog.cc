#include <QDebug>
#include <QFileDialog>
#include "librarypathdialog.hh"


LibraryPathDialog::LibraryPathDialog(QWidget *parent) : QDialog(parent) {
    m_gridLayout = new QGridLayout();
    setLayout(m_gridLayout);

    m_listWidget= new QListWidget(this);
    m_listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    m_listWidget->setSortingEnabled(true);

    m_buttonAdd = new QPushButton(trUtf8("Add"), this);
    m_buttonApply = new QPushButton(trUtf8("Apply"), this);
    m_buttonCancel = new QPushButton(trUtf8("Cancel"), this);
    m_buttonRemove = new QPushButton(trUtf8("Remove"), this);

    connect(m_buttonAdd, SIGNAL(clicked()), this, SLOT(addPath()));
    connect(m_buttonApply, SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_buttonRemove, SIGNAL(clicked()), this, SLOT(removeSelectedItems()));

    m_gridLayout->addWidget(m_listWidget, 0, 0, 1, 4);
    m_gridLayout->addWidget(m_buttonAdd, 1, 0);
    m_gridLayout->addWidget(m_buttonRemove, 1, 1);
    m_gridLayout->addWidget(m_buttonApply, 1, 2);
    m_gridLayout->addWidget(m_buttonCancel, 1, 3);
}


void LibraryPathDialog::addPath() {
    QString caption = trUtf8("Choose path");
    QString pathName = QFileDialog::getExistingDirectory(this, caption, QDir::homePath());
    if (pathName.isEmpty() == false)
        m_listWidget->insertItem(m_listWidget->count(), pathName);
}


QStringList LibraryPathDialog::getPathList() {
    QStringList pathList;
    for (int i = 0; i < m_listWidget->count(); ++i) {
        pathList.append(m_listWidget->item(i)->data(Qt::DisplayRole).toString());
    }

    return pathList;
}


void LibraryPathDialog::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete)
        removeSelectedItems();
    event->accept();
}


void LibraryPathDialog::removeSelectedItems() {
    foreach (QListWidgetItem* listItem, m_listWidget->selectedItems()) {
        m_listWidget->removeItemWidget(listItem);
        delete listItem;
        listItem = 0;
    }
}


void LibraryPathDialog::setPathList(const QStringList& pathList) {
    foreach (const QString& pathName, pathList) {
        m_listWidget->addItem(pathName);
    }
}
