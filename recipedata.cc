#include "recipedata.hh"


QString RecipeData::headline() {
    return m_headline;
}


void RecipeData::clear() {
    m_headline = QString();
    m_ingredientList.clear();
    m_preparationStepList.clear();
    m_servingCount = QString();
}


QList<QMap<QString, QString> > RecipeData::ingredients() {
    return m_ingredientList;
}


QList<QMap<QString, QString> > RecipeData::preparationSteps() {
    return m_preparationStepList;
}


QString RecipeData::servingCount() {
    return m_servingCount;
}


void RecipeData::setHeadline(QString headline) {
    m_headline = headline;
}


void RecipeData::setIngredientList(QList<QMap<QString, QString> > ingredientList) {
    m_ingredientList = ingredientList;
}


void RecipeData::setPreparationStepList(QList<QMap<QString, QString> > preparationStepList) {
    m_preparationStepList = preparationStepList;
}


void RecipeData::setServingCount(QString servingCount) {
    m_servingCount = servingCount;
}
