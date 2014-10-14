#ifndef RECIPEDATA_HH
#define RECIPEDATA_HH


#include <QList>
#include <QMap>
#include <QObject>
#include <QString>


class RecipeData {


public:    

    void clear();
    bool fill(QString fileName);
    QString headline();    
    QList<QMap<QString, QString> > ingredients();
    QList<QMap<QString, QString> > preparationSteps();
    QString servingCount();
    void setHeadline(QString headline);
    void setIngredientList(QList<QMap<QString, QString> > ingredientList);
    void setPreparationStepList(QList<QMap<QString, QString> > preparationStepList);
    void setServingCount(QString servingCount);


private:    

    void addIngredient(QString amount, QString unit, QString name);
    void addPreparationStep(QString stepText);
    void addSection(QString sectionTitle);

    QList<QMap<QString, QString> > m_ingredientList;
    QList<QMap<QString, QString> > m_preparationStepList;
    QString m_headline;    
    QString m_servingCount;

};

#endif // RECIPEDATA_HH
