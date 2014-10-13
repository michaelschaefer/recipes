#ifndef SEARCHFILTERMODEL_HH
#define SEARCHFILTERMODEL_HH


#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include "database.hh"


class SearchFilterModel : public QSortFilterProxyModel {

    Q_OBJECT


public:

    SearchFilterModel(QObject* parent = 0);

    void clearItems();
    void insertItem(Database::RecipeType recipe);


protected:

    bool lessThan(const QModelIndex& left, const QModelIndex& right) const;


private:

    QStandardItemModel* m_internalModel;

};

#endif // SEARCHFILTERMODEL_HH
