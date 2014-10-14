#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QScrollBar>
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
    m_library = Library::instance();
    m_editWidget = new QWidget(this);

    m_previewWidget = new QTextEdit(this);
    m_previewWidget->setReadOnly(true);

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

    m_editArea = new QScrollArea(this);
    m_editArea->setWidget(m_editWidget);
    m_editArea->setWidgetResizable(true);

    insertWidget(0, m_editArea);
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
    QString exportPathName = pathName();
    if (exportPathName.isEmpty() == true)
        Exporter(recipeData()).exportAsPdf();
    else
        Exporter(recipeData()).exportAsPdf(exportPathName);
}


QString RecipeEdit::fileName(bool withPath) {
    if (m_fileName.isEmpty() == true) {
        return trUtf8("unsaved");
    } else {
        if (withPath == true)
            return m_fileName;
        else
            return m_fileName.mid(m_fileName.lastIndexOf(QDir::separator()) + 1);
    }
}


bool RecipeEdit::fromXml(QString fileName) {    
    QByteArray data;
    QFile file(fileName);
    if (file.open(QFile::ReadOnly) == false)
        return false;
    else
        data = file.readAll();

    QString elementName, elementText;
    QXmlStreamReader stream(data);

    if (stream.readNextStartElement() == false || stream.name().toString() != "recipe") {
        return false; // no <recipe> tag
    }

    disconnect(m_ingredients, SIGNAL(changed()), this, SLOT(triggerChanged()));
    disconnect(m_preparation, SIGNAL(changed()), this, SLOT(triggerChanged()));

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

    connect(m_ingredients, SIGNAL(changed()), this, SLOT(triggerChanged()));
    connect(m_preparation, SIGNAL(changed()), this, SLOT(triggerChanged()));

    m_fileName = fileName;
    m_unsavedChanges = false;
    updatePreview();    
    return true;
}


bool RecipeEdit::hasUnsavedChanges() {
    return m_unsavedChanges;
}


QString RecipeEdit::pathName() {
    if (m_fileName.isEmpty() == true)
        return QString();
    else
        return m_fileName.mid(0, m_fileName.lastIndexOf(QDir::separator()));
}


void RecipeEdit::print() {
    Exporter(recipeData()).print();
}


RecipeData& RecipeEdit::recipeData() {
    m_recipeData.setHeadline(m_headline->text());
    m_recipeData.setIngredientList(m_ingredients->data());
    m_recipeData.setPreparationStepList(m_preparation->data());
    m_recipeData.setServingCount(m_ingredients->servingCount());
    return m_recipeData;
}


bool RecipeEdit::save() {
    if (hasUnsavedChanges() == false)
        return true;

    if (m_fileName.isEmpty()) {
        QString caption = trUtf8("Save recipe");
        QString dir = QDir::homePath();
        QString filter = trUtf8("Recipe files (*.xml)");

        QString absoluteFileName = QFileDialog::getSaveFileName(this, caption, dir, filter);
        if (absoluteFileName.isEmpty() == false)
            m_fileName = absoluteFileName;
        else
            return false;
    }

    QFile file(m_fileName);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream << Exporter(recipeData()).xml();
    file.close();

    m_unsavedChanges = false;
    updateLibrary();
    emit saved(this);
    return true;
}


bool RecipeEdit::saveAs() {
    QString caption = trUtf8("Save recipe as");
    QString dir;
    if (m_fileName.isEmpty())
        dir = QDir::homePath();
    else
        dir = m_fileName.mid(0, m_fileName.lastIndexOf(QDir::separator()));
    QString filter = trUtf8("Recipe files (*.xml)");

    QString absoluteFileName = QFileDialog::getSaveFileName(this, caption, dir, filter);
    if (absoluteFileName.isEmpty() == false)
        m_fileName = absoluteFileName;
    else
        return false;

    m_unsavedChanges = true;
    updateLibrary();
    emit saved(this);
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
        m_previewWidget->verticalScrollBar()->triggerAction(QScrollBar::SliderToMinimum);
    }
    m_previewWidget->setVisible(visible);
}


void RecipeEdit::triggerChanged() {
    m_unsavedChanges = true;
    updatePreview();
    emit changed(this);
}


void RecipeEdit::updateLibrary() {
    int lastSeparator = m_fileName.lastIndexOf(QDir::separator());
    QString fileName = m_fileName.mid(lastSeparator + 1);
    QString pathName = m_fileName.mid(0, lastSeparator);
    m_library->insertOrUpdateFile(fileName, pathName, recipeData());
}


void RecipeEdit::updatePreview() {
    m_previewWidget->setDocument(Exporter(recipeData(), this).textEdit()->document());
}
