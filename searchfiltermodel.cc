#include "searchfiltermodel.hh"


SearchFilterModel::SearchFilterModel(QObject *parent) : QSortFilterProxyModel(parent) {
    m_internalModel = new QStandardItemModel(parent);
    m_internalModel->setColumnCount(1);
    setSourceModel(m_internalModel);
    this->sort(0);
}


void SearchFilterModel::clearItems() {
    while (m_internalModel->rowCount() > 0)
        m_internalModel->removeRow(0);
}


void SearchFilterModel::insertItem(Database::RecipeType recipe) {
    int pos = m_internalModel->rowCount();
    m_internalModel->insertRow(pos);
    QModelIndex index = m_internalModel->index(pos, 0);
    m_internalModel->setData(index, recipe.first, Qt::DisplayRole);
    m_internalModel->setData(index, recipe.second, Qt::UserRole);
}


bool SearchFilterModel::lessThan(const QModelIndex& left, const QModelIndex& right) const {
    QString leftDisplay = left.data().toString();
    QString rightDisplay = right.data().toString();

    if (leftDisplay == rightDisplay) {
        return (left.data(Qt::UserRole) < right.data(Qt::UserRole));
    } else
        return (leftDisplay < rightDisplay);
}



