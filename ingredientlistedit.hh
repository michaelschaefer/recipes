#ifndef INGREDIENTLISTEDIT_HH
#define INGREDIENTLISTEDIT_HH


#include "flexiblelayout.hh"


class IngredientListEdit : public QWidget {

    Q_OBJECT


signals:

    void changed();


public:

    IngredientListEdit(QWidget* parent = 0);

    void addIngredient();
    void addIngredient(QString amount, QString unit, QString name);
    void addSection();
    void addSection(QString sectionName);
    QList<QMap<QString, QString> > data();
    void editServingCount();
    QString servingCount();
    void updateHeadline();


private slots:

    void triggerChanged();


private:

    FlexibleLayout* m_flexibleLayout;
    QFont m_boldFont;
    QLabel* m_headline;
    QString m_servingCount;
    QVBoxLayout* m_layout;


};

#endif // INGREDIENTLISTEDIT_HH
