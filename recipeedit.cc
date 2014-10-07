#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QTextCodec>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "exporter.hh"
#include "recipeedit.hh"


RecipeEdit::RecipeEdit(QWidget *parent)
    : QSplitter(parent)
{
    setupFonts();

    m_unsavedChanges = true;
    m_editWidget = new QWidget(this);
    m_previewWidget = new QTextEdit(this);

    m_headline = new QLabel(trUtf8("unnamed"), m_editWidget);
    m_headline->setAlignment(Qt::AlignHCenter);
    m_headline->setFont(m_h1Font);

    m_ingredients = new IngredientListEdit(m_editWidget);
    m_preparation = new PreparationListEdit(m_editWidget);

    m_layout = new QVBoxLayout();
    m_layout->addWidget(m_headline);
    m_layout->addWidget(m_ingredients);
    m_layout->addWidget(m_preparation);
    m_layout->addStretch(100);
    m_editWidget->setLayout(m_layout);

    insertWidget(0, m_editWidget);
    insertWidget(1, m_previewWidget);

    // preview
    updatePreview();
    togglePreview(false);

    connect(m_ingredients, SIGNAL(changed()), this, SLOT(triggerChanged()));
    connect(m_preparation, SIGNAL(changed()), this, SLOT(triggerChanged()));
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
    QString title = trUtf8("Edit headline");

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


void RecipeEdit::exportAsPdf() {
    Exporter(recipeData()).print(true);
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
    updatePreview();

    return true;
}


bool RecipeEdit::hasUnsavedChanges() {
    return m_unsavedChanges;
}


void RecipeEdit::print() {
    Exporter(recipeData()).print();
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
        QString caption = trUtf8("Save recipe");
        QString dir = QDir::homePath();
        QString filter = trUtf8("Recipe files (*.xml)");

        QString filename = QFileDialog::getSaveFileName(this, caption, dir, filter);
        if (filename.isEmpty() == false)
            m_filename = filename;
        else
            return false;
    }

    QFile file(m_filename);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream << Exporter(recipeData()).xml();
    file.close();

    m_unsavedChanges = false;
    emit saved(this);
    return true;
}


bool RecipeEdit::saveAs() {
    QString caption = trUtf8("Save recipe as");
    QString dir = QDir::homePath();
    QString filter = trUtf8("Recipe files (*.xml)");

    QString filename = QFileDialog::getSaveFileName(this, caption, dir, filter);
    if (filename.isEmpty() == false)
        m_filename = filename;
    else
        return false;

    m_unsavedChanges = true;
    return save();
}


void RecipeEdit::setupFonts() {
    double basePointSize = font().pointSize();

    m_h1Font.setWeight(QFont::Bold);
    m_h1Font.setPointSize(basePointSize * 1.6);

    m_h2Font.setWeight(QFont::Bold);
    m_h2Font.setPointSize(basePointSize);
}


void RecipeEdit::togglePreview(bool visible) {
    if (visible == true) {
        int w = width();
        int half = (w - (w % 2)) / 2;
        QList<int> sizeList;
        sizeList.append(half);
        sizeList.append(w - half);
        setSizes(sizeList);
    }
    m_previewWidget->setVisible(visible);
}


void RecipeEdit::triggerChanged() {
    m_unsavedChanges = true;
    updatePreview();
    emit changed(this);
}


void RecipeEdit::updatePreview() {
    m_previewWidget->setDocument(Exporter(recipeData(), this).textEdit()->document());
}
