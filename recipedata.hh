#ifndef RECIPEDATA_HH
#define RECIPEDATA_HH


#include <QList>
#include <QMap>
#include <QString>


class RecipeData {

public:

    QString headline();
    QList<QMap<QString, QString> > ingredients();
    QList<QMap<QString, QString> > preparationSteps();
    QString servingCount();
    void setHeadline(QString headline);
    void setIngredientList(QList<QMap<QString, QString> > ingredientList);
    void setPreparationStepList(QList<QMap<QString, QString> > preparationStepList);
    void setServingCount(QString servingCount);


private:

    QList<QMap<QString, QString> > m_ingredientList;
    QList<QMap<QString, QString> > m_preparationStepList;
    QString m_headline;
    QString m_servingCount;

};

#endif // RECIPEDATA_HH
