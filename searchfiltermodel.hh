#ifndef SEARCHFILTERMODEL_HH
#define SEARCHFILTERMODEL_HH


#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include "database.hh"


class SearchFilterModel : public QSortFilterProxyModel {

    Q_OBJECT


public:

    SearchFilterModel(QObject* parent = 0);

    void clearFilter();
    void clearItems();
    int getFileId(QModelIndex index);
    void insertItem(Database::Recipe recipe);    


public slots:

    void filter(QString headlineFilter, QList<QList<int> > ingredientIdFilter);


protected:

    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const;


private:

    bool m_filterActive;
    QList<QList<int> > m_ingredientIdFilter;
    QStandardItemModel* m_internalModel;
    QString m_headlineFilter;

};

#endif // SEARCHFILTERMODEL_HH
