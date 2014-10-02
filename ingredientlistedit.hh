#ifndef INGREDIENTLISTEDIT_HH
#define INGREDIENTLISTEDIT_HH


#include <QFont>
#include <QLabel>
#include <QList>
#include <QVBoxLayout>
#include <QWidget>
#include "flexiblelayout.hh"
#include "widgetinterface.hh"


class IngredientListEdit : public QWidget {

    Q_OBJECT


signals:

    void changed();


public:

    typedef QList<WidgetInterface::DataType> DataType;

    IngredientListEdit(QWidget* parent = 0);

    void addIngredient();
    void addIngredient(QString amount, QString unit, QString name);
    void addSection();
    void addSection(QString sectionName);
    IngredientListEdit::DataType data();
    void editServingCount();


private slots:

    void triggerChanged();


private:

    void updateHeadline();


    FlexibleLayout* m_flexibleLayout;
    QFont m_boldFont;
    QLabel* m_headline;
    QString m_servingCount;
    QVBoxLayout* m_layout;


};

#endif // INGREDIENTLISTEDIT_HH
