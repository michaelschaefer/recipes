#ifndef SEARCHFILTERWIDGET_HH
#define SEARCHFILTERWIDGET_HH


#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QListWidget>


class SearchFilterWidget : public QWidget {

    Q_OBJECT


public:

    struct FilterSettings {
        Qt::CaseSensitivity caseSensitivity;
    };

    SearchFilterWidget(QWidget* parent = 0);

    void clearFilter();
    void setFocus();    


signals:

    void filterSettingsUpdated(SearchFilterWidget::FilterSettings);
    void filterUpdated(QString, QList<QList<int> >);


private slots:

    void editCurrentListItem();
    bool eventFilter(QObject* target, QEvent* event);
    void insertIngredient();
    void removeCurrentListItem();
    void updateFilter();
    void updateFilterSettings();
    void updateIngredientIdList();


private:

    void setupPopupMenu();

    FilterSettings m_filterSettings;
    QAction* m_actionEdit;
    QAction* m_actionRemove;
    QCheckBox* m_checkBoxCaseSensitive;
    QFormLayout* m_layoutInput;
    QGroupBox* m_groupBoxSearchSettings;
    QLineEdit* m_editHeadline;
    QLineEdit* m_lineEditIngredients;
    QList<QList<int> > m_ingredientIdList;
    QListWidget* m_listWidgetIngredients;
    QMenu* m_popupMenu;
    QStringList m_ingredientNameList;
    QVBoxLayout* m_layoutIngredients;
    QVBoxLayout* m_layout;

};

#endif // SEARCHFILTERWIDGET_HH
