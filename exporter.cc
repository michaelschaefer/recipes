#include <QFileDialog>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QTextCodec>
#include <QTextList>
#include <QTextTable>
#include <QXmlStreamWriter>
#include "exporter.hh"


void Exporter::exportAsPdf(QString dir) {
    QString title = trUtf8("Export as PDF");
    QString filter = trUtf8("PDF documents (*.pdf)");
    QString fileName = QFileDialog::getSaveFileName(m_parent, title, dir, filter);
    if (fileName.isEmpty() == false) {
        QPrinter* printer = new QPrinter(QPrinter::HighResolution);
        printer->setFontEmbeddingEnabled(true);
        printer->setOutputFileName(fileName);
        printer->setOutputFormat(QPrinter::PdfFormat);
        printer->setPaperSize(QPrinter::A4);
        printDocument(printer);
    }
}


Exporter* Exporter::instance() {
    static Exporter instance;
    return &instance;
}


void Exporter::print() {
    QPrintPreviewDialog previewDialog(m_parent);
    previewDialog.setModal(true);
    connect(&previewDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printDocument(QPrinter*)));
    previewDialog.show();
    previewDialog.exec();
}


void Exporter::printDocument(QPrinter* printer) {
    QTextEdit* textEdit = this->textEdit();
    if (textEdit != 0)
        textEdit->document()->print(printer);
}


void Exporter::setRecipeData(RecipeData& recipeData) {
    m_recipeData = recipeData;
}


QTextEdit* Exporter::textEdit() {
    QTextEdit* textEdit = new QTextEdit();
    textEdit->setReadOnly(true);

    /*
     * setup fonts
     */

    int id = QFontDatabase::addApplicationFont(":/font/gentium");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);

    double basePointSize = 12;
    QFont defaultFont(family), h1(family), h2(family);

    defaultFont.setWeight(QFont::Normal);
    defaultFont.setPointSize(basePointSize);

    h1.setWeight(QFont::Bold);
    h1.setPointSize(basePointSize * 1.6);

    h2.setWeight(QFont::Bold);
    h2.setPointSize(basePointSize);


    /*
     * formats
     */

    QTextBlockFormat doubleSpacingFormat;
    doubleSpacingFormat.setLineHeight(200, QTextBlockFormat::ProportionalHeight);

    QTextBlockFormat normalSpacingFormat;
    normalSpacingFormat.setLineHeight(100, QTextBlockFormat::ProportionalHeight);

    QTextBlockFormat oneHalfSpacingFormat;
    oneHalfSpacingFormat.setLineHeight(120, QTextBlockFormat::ProportionalHeight);

    QTextCharFormat defaultFormat;
    defaultFormat.setFont(defaultFont);

    QTextCharFormat h1Format;
    h1Format.setFont(h1);

    QTextCharFormat h2Format;
    h2Format.setFont(h2);

    QTextListFormat listFormat;
    listFormat.setStyle(QTextListFormat::ListDecimal);
    listFormat.setNumberSuffix(". ");


    /*
     * recipe
     */

    QTextCursor cursor(textEdit->textCursor());

    // headline
    textEdit->setAlignment(Qt::AlignHCenter);
    textEdit->setFont(defaultFont);
    cursor.insertText(m_recipeData.headline(), h1Format);
    cursor.insertBlock(oneHalfSpacingFormat, defaultFormat);

    // ingredients headline
    cursor.insertBlock(doubleSpacingFormat);
    cursor.insertText(trUtf8("Ingredients"), h2Format);
    if (m_recipeData.servingCount().isEmpty() == false) {
        cursor.insertText(" " + m_recipeData.servingCount());
    }

    // ingredient list
    Exporter::EntryListType ingredients = m_recipeData.ingredients();
    int size = ingredients.size();
    for (int i = 0; i < size; ++i) {
        Exporter::EntryType entry = ingredients[i];
        if (i < size-1 && ingredients[i+1]["type"] == "section") {
            cursor.insertBlock(doubleSpacingFormat);
        } else {
            cursor.insertBlock(oneHalfSpacingFormat);
        }

        if (entry["type"] == "section") {
            cursor.insertText(entry["title"], h2Format);
        } else if (entry["type"] == "ingredient") {
            if (entry["unit"].isEmpty() == true)
                cursor.insertText(entry["amount"] + "\t" + entry["name"], defaultFormat);
            else
                cursor.insertText(entry["amount"] + " " + entry["unit"] + "\t" + entry["name"], defaultFormat);
        }
    }
    cursor.insertBlock(oneHalfSpacingFormat);


    // preparation headline
    cursor.insertBlock(doubleSpacingFormat);
    cursor.insertText(trUtf8("Preparation"), h2Format);

    oneHalfSpacingFormat.setBottomMargin(basePointSize);
    cursor.insertBlock(oneHalfSpacingFormat, defaultFormat);

    // preparation steps
    cursor.createList(listFormat);
    Exporter::EntryListType preparationSteps = m_recipeData.preparationSteps();
    for (int i = 0; i < preparationSteps.size(); ++i) {
        Exporter::EntryType entry = preparationSteps[i];
        if (entry["type"] == "preparationStep") {
            if (i > 0) {
                cursor.insertBlock();
            }
            cursor.insertText(entry["text"]);
        }
    }

    return textEdit;
}


QString Exporter::xml() {
    Exporter::EntryListType ingredientList = m_recipeData.ingredients();
    Exporter::EntryListType preparationStepList = m_recipeData.preparationSteps();

    QString xml;
    QXmlStreamWriter stream(&xml);

    // document start
    stream.setAutoFormatting(true);
    stream.setCodec(QTextCodec::codecForName("UTF-8"));
    stream.writeStartDocument();
    stream.writeStartElement("recipe");

    // headline
    stream.writeTextElement("title", m_recipeData.headline());

    // ingredients
    stream.writeStartElement("ingredients");
    for (Exporter::EntryListType::Iterator it = ingredientList.begin(); it != ingredientList.end(); ++it) {
        Exporter::EntryType entry = *it;
        if (entry["type"] == "ingredient") {
            stream.writeStartElement("ingredient");
            stream.writeTextElement("amount", entry["amount"]);
            stream.writeTextElement("unit", entry["unit"]);
            stream.writeTextElement("name", entry["name"]);
            stream.writeEndElement();
        } else if (entry["type"] == "section") {
            stream.writeTextElement("section", entry["title"]);
        }
    }
    stream.writeEndElement();

    // preparation
    stream.writeStartElement("preparation");
    for (Exporter::EntryListType::Iterator it = preparationStepList.begin(); it != preparationStepList.end(); ++it) {
        Exporter::EntryType entry = *it;
        stream.writeTextElement("step", entry["text"]);
    }
    stream.writeEndElement();

    // document end
    stream.writeEndElement();
    stream.writeEndDocument();

    return xml;
}
