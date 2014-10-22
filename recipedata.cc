#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include "recipedata.hh"


void RecipeData::addIngredient(QString amount, QString unit, QString name) {
    QMap<QString, QString> ingredientMap;
    ingredientMap["type"] = "ingredient";
    ingredientMap["amount"] = amount;
    ingredientMap["unit"] = unit;
    ingredientMap["name"] = name;
    m_ingredientList.append(ingredientMap);
}


void RecipeData::addPreparationStep(QString stepText) {
    QMap<QString, QString> preparationStepMap;
    preparationStepMap["type"] = "preparationStep";
    preparationStepMap["text"] = stepText;
    m_preparationStepList.append(preparationStepMap);
}


void RecipeData::addSection(QString sectionTitle) {
    QMap<QString, QString> sectionMap;
    sectionMap["type"] = "section";
    sectionMap["title"] = sectionTitle;
    m_ingredientList.append(sectionMap);
}


bool RecipeData::fill(QString fileName) {    
    QByteArray data;
    QFile file(fileName);
    if (file.open(QFile::ReadOnly) == false)
        return false;
    else {
        data = file.readAll();
        file.close();
    }

    QString elementName, elementText;
    QXmlStreamReader stream(data);

    if (stream.readNextStartElement() == false || stream.name().toString() != "recipe")
        return false; // no <recipe> tag

    while (stream.atEnd() == false) {
        if (stream.readNextStartElement() == false)
            continue;

        elementName = stream.name().toString();

        /*
         * <title> section
         */
        if (elementName == "title") {
            elementText = stream.readElementText();
            if (stream.hasError() == true)
                return false;
            else
                setHeadline(elementText);
        }

        /*
         * <ingredients> section
         */
        if (elementName == "ingredients") {
            while (stream.readNextStartElement() == true) {
                elementName = stream.name().toString();
                if (elementName == "ingredient") {
                    bool hasAmount = false, hasName = false, hasUnit = false;
                    QString amount, name, unit;
                    while (stream.readNextStartElement() == true) {
                        elementName = stream.name().toString();
                        elementText = stream.readElementText();
                        if (stream.hasError() == true)
                            return false;
                        else {
                            if (elementName == "amount") {
                                amount = elementText;
                                hasAmount = true;
                            } else if (elementName == "name") {
                                name = elementText;
                                hasName = true;
                            } else if (elementName == "unit") {
                                unit = elementText;
                                hasUnit = true;
                            } else
                                return false;
                        }
                    }

                    if (hasAmount == false || hasName == false || hasUnit == false)
                        return false;
                    else
                        addIngredient(amount, unit, name);
                    elementName = "ingredient";
                } else if (elementName == "section") {
                    elementText = stream.readElementText();
                    if (stream.hasError() == true)
                        return false;
                    else
                        addSection(elementText);
                } else
                    return false;
            }

            elementName = "ingredients";

        }

        /*
         * <preparation> section
         */
        if (elementName == "preparation") {
            while (stream.readNextStartElement() == true) {
                elementName = stream.name().toString();
                if (elementName == "step") {
                    elementText = stream.readElementText();
                    if (stream.hasError() == true)
                        return false;
                    else
                        addPreparationStep(elementText);
                } else
                    return false;
            }
        }
    }

    return true;
}


QString RecipeData::headline() {
    return m_headline;
}


QStringList RecipeData::ingredientNameList() {
    QStringList ingredientNameList;
    QMap<QString, QString> ingredient;
    foreach (ingredient, m_ingredientList) {
        if (ingredient["type"] == "ingredient")
            ingredientNameList << ingredient["name"];
    }
    return ingredientNameList;
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
