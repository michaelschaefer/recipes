#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QTextCodec>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "recipeedit.hh"


RecipeEdit::RecipeEdit(QWidget *parent)
    : QWidget(parent)
{
    setupFonts();

    m_unsavedChanges = true;

    m_headline = new QLabel(trUtf8("<unnamed>"), this);
    m_headline->setAlignment(Qt::AlignHCenter);
    m_headline->setFont(m_h1Font);

    m_ingredients = new IngredientListEdit(this);    
    m_preparation = new PreparationListEdit(this);
    connect(m_ingredients, SIGNAL(changed()), this, SLOT(triggerChanged()));
    connect(m_preparation, SIGNAL(changed()), this, SLOT(triggerChanged()));

    m_layout = new QVBoxLayout();
    m_layout->addWidget(m_headline);
    m_layout->addWidget(m_ingredients);
    m_layout->addWidget(m_preparation);
    m_layout->addStretch(100);
    setLayout(m_layout);
}


void RecipeEdit::addIngredient() {
    m_ingredients->addIngredient();
}


void RecipeEdit::addPreparationStep() {
    m_preparation->addPreparationStep();
}


void RecipeEdit::addSection() {
    m_ingredients->addSection();
}


void RecipeEdit::editHeadline() {
    bool okay;
    QString label = trUtf8("Input new headline for the recipe:");
    QString text = m_headline->text().replace("\n", "\\n");
    QString title = trUtf8("Edit Headline");    

    QString headline = QInputDialog::getText(this, title, label, QLineEdit::Normal, text, &okay);
    if (okay == true && headline != m_headline->text()) {
        headline.replace("\\n", "\n");
        m_headline->setText(headline);
        triggerChanged();
    }
}


void RecipeEdit::editServingCount() {
    m_ingredients->editServingCount();
}


QString RecipeEdit::filename(bool withPath) {
    if (m_filename.isEmpty() == true) {
        return trUtf8("unsaved");
    } else {
        if (withPath == true)
            return m_filename;
        else
            return m_filename.mid(m_filename.lastIndexOf("/") + 1);
    }
}


bool RecipeEdit::fromXml(QString filename) {
    QFile file(filename);
    if (file.open(QFile::ReadOnly) == false) {
        return false;
    }

    QString elementName, elementText;
    QXmlStreamReader stream(&file);


    if (stream.readNextStartElement() == false || stream.name().toString() != "recipe") {
        return false; // no <recipe> tag
    }

    while (stream.atEnd() == false) {
        if (stream.readNextStartElement() == true) {

            elementName = stream.name().toString();
            if (elementName == "title") {
                elementText = stream.readElementText();
                if (stream.hasError() == true)
                    return false;
                else
                    m_headline->setText(elementText);
            } else if (elementName == "ingredients") {

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

                        if (hasAmount == true && hasName == true && hasUnit == true)
                            m_ingredients->addIngredient(amount, unit, name);
                        else
                            return false;
                        elementName = "ingredient";

                    } else if (elementName == "section") {
                        elementText = stream.readElementText();
                        if (stream.hasError() == true)
                            return false;
                        else
                            m_ingredients->addSection(elementText);
                    } else {
                        return false;
                    }
                }

                elementName = "ingredients";

            } else if (elementName == "preparation") {

                while (stream.readNextStartElement() == true) {
                    elementName = stream.name().toString();
                    if (elementName == "step") {
                        elementText = stream.readElementText();
                        if (stream.hasError() == true)
                            return false;
                        else
                            m_preparation->addPreparationStep(elementText);
                    } else
                        return false;
                }

            }

        }
    }

    m_filename = filename;
    m_unsavedChanges = false;

    return true;
}


bool RecipeEdit::hasUnsavedChanges() {
    return m_unsavedChanges;
}


RecipeData RecipeEdit::recipeData() {
    RecipeData recipeData;
    recipeData.setHeadline(m_headline->text());
    recipeData.setIngredientList(m_ingredients->data());
    recipeData.setPreparationStepList(m_preparation->data());
    recipeData.setServingCount(m_ingredients->servingCount());
    return recipeData;
}


bool RecipeEdit::save() {
    if (hasUnsavedChanges() == false)
        return true;

    if (m_filename.isEmpty()) {
        QString caption = trUtf8("Save Recipe");
        QString dir = QDir::homePath();
        QString filter = trUtf8("Recipe files (*.xml)");
        QString filename = QFileDialog::getSaveFileName(this, caption, dir, filter);

        if (filename.isEmpty() == false) {
            m_filename = filename;
        } else {
            return false;
        }
    }

    QFile file(m_filename);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream << toXml();
    file.close();

    m_unsavedChanges = false;
    return true;
}


void RecipeEdit::setupFonts() {
    double basePointSize = font().pointSize();

    m_h1Font.setWeight(QFont::Bold);
    m_h1Font.setPointSize(basePointSize * 1.6);

    m_h2Font.setWeight(QFont::Bold);
    m_h2Font.setPointSize(basePointSize);
}


void RecipeEdit::triggerChanged() {
    m_unsavedChanges = true;
    emit changed(this);
}


QString RecipeEdit::toXml() {
    IngredientListEdit::DataType ingredientData = m_ingredients->data();
    PreparationListEdit::DataType preparationData = m_preparation->data();
    QString xml;
    QXmlStreamWriter stream(&xml);

    // document start
    stream.setAutoFormatting(true);
    stream.setCodec(QTextCodec::codecForName("UTF-8"));
    stream.writeStartDocument();
    stream.writeStartElement("recipe");

    // headline
    stream.writeTextElement("title", m_headline->text());

    // ingredients
    WidgetInterface::DataType item;
    stream.writeStartElement("ingredients");

    for (IngredientListEdit::DataType::Iterator it = ingredientData.begin(); it != ingredientData.end(); ++it) {
        item = *it;
        if (item["type"] == "ingredient") {
            stream.writeStartElement("ingredient");
            stream.writeTextElement("amount", item["amount"]);
            stream.writeTextElement("unit", item["unit"]);
            stream.writeTextElement("name", item["name"]);
            stream.writeEndElement();
        } else if (item["type"] == "section") {
            stream.writeTextElement("section", item["title"]);
        }
    }

    stream.writeEndElement();

    // preparation
    WidgetInterface::DataType preparationStep;
    stream.writeStartElement("preparation");
    for (PreparationListEdit::DataType::Iterator it = preparationData.begin(); it != preparationData.end(); ++it) {
        preparationStep = *it;
        stream.writeTextElement("step", preparationStep["text"]);
    }
    stream.writeEndElement();

    // document end
    stream.writeEndElement();
    stream.writeEndDocument();

    return xml;
}
