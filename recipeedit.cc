#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QList>
#include <QMap>
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
    m_exporter = Exporter::instance();

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
    m_exporter->setRecipeData(recipeData());    
    m_exporter->exportAsPdf(fileName(), pathName());
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


bool RecipeEdit::fill(QString fileName) {
    RecipeData recipeData;
    if (recipeData.fill(fileName) == false)
        return false;

    disconnect(m_ingredients, SIGNAL(changed()), this, SLOT(triggerChanged()));
    disconnect(m_preparation, SIGNAL(changed()), this, SLOT(triggerChanged()));

    m_headline->setText(recipeData.headline());

    QMap<QString, QString> ingredient;
    foreach (ingredient, recipeData.ingredients()) {
        if (ingredient["type"] == "ingredient")
            m_ingredients->addIngredient(ingredient["amount"], ingredient["unit"], ingredient["name"]);
        else if (ingredient["type"] == "section")
            m_ingredients->addSection(ingredient["title"]);
    }

    QMap<QString, QString> preparationStep;
    foreach(preparationStep, recipeData.preparationSteps()) {
        if (preparationStep["type"] == "preparationStep")
            m_preparation->addPreparationStep(preparationStep["text"]);
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
    m_exporter->setRecipeData(recipeData());
    m_exporter->print();
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

    m_exporter->setRecipeData(recipeData());

    QFile file(m_fileName);
    QTextStream stream(&file);
    file.open(QIODevice::WriteOnly);    
    stream << m_exporter->xml();
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
    m_library->insertOrUpdateFile(fileName, recipeData());
}


void RecipeEdit::updatePreview() {
    m_exporter->setRecipeData(recipeData());
    m_previewWidget->setDocument(m_exporter->textEdit()->document());
}
