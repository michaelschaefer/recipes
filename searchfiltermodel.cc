#include "searchfiltermodel.hh"


SearchFilterModel::SearchFilterModel(QObject *parent) : QSortFilterProxyModel(parent) {
    m_filterActive = false;
    m_internalModel = new QStandardItemModel(parent);
    m_internalModel->setColumnCount(3);
    setSourceModel(m_internalModel);
    this->sort(0);
}


void SearchFilterModel::clearItems() {
    while (m_internalModel->rowCount() > 0)
        m_internalModel->removeRow(0);
}


void SearchFilterModel::clearFilter() {
    m_headlineFilter.clear();
    m_ingredientIdFilter.clear();
    m_filterActive = false;
}


void SearchFilterModel::filter(QString headlineFilter, QList<QList<int> > ingredientIdFilter) {
    m_filterActive = true;
    m_headlineFilter = headlineFilter.trimmed();
    m_ingredientIdFilter = ingredientIdFilter;
    invalidateFilter();
}


bool SearchFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
    QModelIndex index;

    if (m_filterActive == false)
        return true;
    if (m_ingredientIdFilter.size() == 1 && m_ingredientIdFilter[0].isEmpty() == true)
        return false;

    if (m_headlineFilter.isEmpty() == false) {
        index = sourceModel()->index(sourceRow, 0, sourceParent);
        if (sourceModel()->data(index).toString().contains(m_headlineFilter) == false)
            return false;
    }

    if (m_ingredientIdFilter.isEmpty() == false) {
        index = sourceModel()->index(sourceRow, 2, sourceParent);
        QList<QVariant> ingredientIdList = sourceModel()->data(index).toList();
        /*foreach (int ingredientId, m_ingredientIdFilter) {
            if (ingredientList.contains(QVariant(ingredientId)) == false)
                return false;
        }*/
        foreach (const QList<int>& availableIdList, m_ingredientIdFilter) {
            bool containsAtLeastOne = false;
            foreach (int availableId, availableIdList) {
                if (ingredientIdList.contains(QVariant(availableId)) == true) {
                    containsAtLeastOne = true;
                    break;
                }
            }
            if (containsAtLeastOne == false)
                return false;
        }
    }

    return true;
}


int SearchFilterModel::getFileId(QModelIndex index) {
    return data(this->index(index.row(), 1)).toInt();
}


void SearchFilterModel::insertItem(Database::Recipe recipe) {
    int pos = m_internalModel->rowCount();
    m_internalModel->insertRow(pos);

    QModelIndex index;
    index = m_internalModel->index(pos, 0);
    m_internalModel->setData(index, recipe.headline);

    index = m_internalModel->index(pos, 1);
    m_internalModel->setData(index, recipe.fileId);

    index = m_internalModel->index(pos, 2);
    QList<QVariant> list;
    for (int i = 0; i < recipe.ingredientIdList.size(); ++i)
        list.append(recipe.ingredientIdList[i]);    
    m_internalModel->setData(index, list);
}


bool SearchFilterModel::lessThan(const QModelIndex& left, const QModelIndex& right) const {
    QString leftHeadline = left.data().toString();
    QString rightHeadline = right.data().toString();

    if (leftHeadline == rightHeadline) {
        QModelIndex leftId = index(left.row(), 1);
        QModelIndex rightId = index(right.row(), 1);
        return (leftId.data() < rightId.data());
    } else
        return (leftHeadline < rightHeadline);
}




