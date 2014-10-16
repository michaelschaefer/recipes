#ifndef SEARCHFILTERWIDGET_HH
#define SEARCHFILTERWIDGET_HH


#include <QFormLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QWidget>


class SearchFilterWidget : public QWidget {

    Q_OBJECT


signals:

    void filterUpdated(QString, QList<QList<int> >);


public:

    SearchFilterWidget(QWidget* parent = 0);

    QString headlineFilter();
    QStringList ingredientFilter();
    void setFocus();


private slots:

    bool eventFilter(QObject* target, QEvent* event);
    void updateFilter();
    void updateIngredientIdList();


private:

    QFormLayout* m_layout;
    QLineEdit* m_editHeadline;
    QList<QList<int> > m_ingredientIdList;
    QPlainTextEdit* m_editIngredients;

};

#endif // SEARCHFILTERWIDGET_HH
