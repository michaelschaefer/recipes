#include <QDebug>
#include "synchronizationdialog.hh"


SynchronizationDialog::SynchronizationDialog(QWidget* parent) : QDialog(parent) {
    setupButtonBox();

    setWindowTitle(trUtf8("Synchronize"));

    m_labelHint = new QLabel(this);
    m_labelHint->setText(trUtf8("The following files were found both local and remote. Check the local file to upload, the remote one to download or none to ignore it."));
    m_labelHint->setWordWrap(true);

    m_treeView = new QTreeView(this);
    m_treeViewModel = new QStandardItemModel();
    m_treeView->setModel(m_treeViewModel);

    m_layout = new QVBoxLayout();
    m_layout->addWidget(m_labelHint);
    m_layout->addWidget(m_treeView);
    m_layout->addWidget(m_buttonBox);
    setLayout(m_layout);

    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_treeViewModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)));
    connect(m_buttonDownload, SIGNAL(clicked()), this, SLOT(downloadAll()));
    connect(m_buttonUpload, SIGNAL(clicked()), this, SLOT(uploadAll()));
}


void SynchronizationDialog::downloadAll() {
    for (int i = 0; i < m_treeViewModel->rowCount(); ++i)
        m_treeViewModel->setData(m_treeViewModel->index(i, 3), Qt::Checked, Qt::CheckStateRole);
}


QStringList SynchronizationDialog::downloadFileList() {
    QStringList downloadFileList;
    QModelIndex index;
    for (int i = 0; i < m_treeViewModel->rowCount(); ++i) {
        index = m_treeViewModel->index(i, 3);
        if (m_treeViewModel->itemFromIndex(index)->checkState() == Qt::Checked)
            downloadFileList.append(m_treeViewModel->data(index).toString());
    }
    return downloadFileList;
}


void SynchronizationDialog::itemChanged(QStandardItem* item) {
    QModelIndex index = m_treeViewModel->indexFromItem(item);
    int column = index.column();
    if (column == 0 || column == 3) {
        int otherColumn = (column == 0) ? 3 : 0;
        if (item->checkState() == Qt::Checked)
            m_treeViewModel->setData(m_treeViewModel->index(index.row(), otherColumn), Qt::Unchecked, Qt::CheckStateRole);
    }
}


void SynchronizationDialog::initView() {
    // set header labels
    QStringList list;
    list << "Local" << "modified" << "size" << "Remote" << "modified" << "size";
    m_treeViewModel->setHorizontalHeaderLabels(list);

    // set data
    QList<QStandardItem*> itemList;
    QStandardItem* item;
    int size = m_syncDataList.size();
    for (int i = 0; i < size; ++i) {
        itemList.clear();
        item = new QStandardItem(m_syncDataList[i][0]);
        item->setCheckable(true);
        itemList << item;
        itemList << new QStandardItem(m_syncDataList[i][1]);
        itemList << new QStandardItem(m_syncDataList[i][2]);
        item = new QStandardItem(m_syncDataList[i][0]);
        item->setCheckable(true);
        itemList << item;
        itemList << new QStandardItem(m_syncDataList[i][3]);
        itemList << new QStandardItem(m_syncDataList[i][4]);
        m_treeViewModel->insertRow(m_treeViewModel->rowCount(), itemList);
    }

    // make the view non-editable and remove indentation in first column
    m_treeView->setIndentation(0);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    for (int i = 0; i < m_treeViewModel->columnCount(); ++i) {
        if (i == 0 || i == 3)
            continue;
        m_treeView->resizeColumnToContents(i);
    }
}


void SynchronizationDialog::setData(QList<QStringList> syncDataList) {
    m_syncDataList = syncDataList;
    initView();
}


void SynchronizationDialog::setupButtonBox() {
    m_buttonBox = new QDialogButtonBox(this);

    m_buttonDownload = new QPushButton(trUtf8("Select all for download"), this);
    m_buttonUpload = new QPushButton(trUtf8("Select all for upload"), this);

    m_buttonBox->addButton(m_buttonDownload, QDialogButtonBox::ActionRole);
    m_buttonBox->addButton(m_buttonUpload, QDialogButtonBox::ActionRole);
    m_buttonBox->addButton(QDialogButtonBox::Ok);
    m_buttonBox->addButton(QDialogButtonBox::Cancel);
}


void SynchronizationDialog::uploadAll() {
    for (int i = 0; i < m_treeViewModel->rowCount(); ++i)
        m_treeViewModel->setData(m_treeViewModel->index(i, 0), Qt::Checked, Qt::CheckStateRole);
}

QStringList SynchronizationDialog::uploadFileList() {
    QStringList uploadFileList;
    QModelIndex index;
    for (int i = 0; i < m_treeViewModel->rowCount(); ++i) {
        index = m_treeViewModel->index(i, 0);
        if (m_treeViewModel->itemFromIndex(index)->checkState() == Qt::Checked)
            uploadFileList.append(m_treeViewModel->data(index).toString());
    }
    return uploadFileList;
}
