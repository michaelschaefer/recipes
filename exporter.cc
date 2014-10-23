#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QProgressDialog>
#include <QTextCodec>
#include <QTextList>
#include <QTextTable>
#include <QXmlStreamWriter>
#include <QSettings>
#include "exporter.hh"
#include "library.hh"


Exporter::Exporter() {}


void Exporter::exportAsPdf(QString absoluteFileName, bool withPrompt) {
    if (absoluteFileName.isEmpty() == true || withPrompt == true) {
        QString caption = trUtf8("Export as PDF");
        QString filter = trUtf8("PDF documents (*.pdf)");
        QString dir = absoluteFileName;
        if (dir.isEmpty() == true) {
            dir = QSettings().value("library/local/path").toString();
            if (dir.isEmpty() == true)
                dir = QDir::homePath();
        } else
            dir = dir.mid(0, dir.lastIndexOf(".")) + ".pdf";

        absoluteFileName = QFileDialog::getSaveFileName(0, caption, dir, filter);
        if (absoluteFileName.isEmpty() == true)
            return;
    }

    if (absoluteFileName.endsWith(".xml") == true)
        absoluteFileName.replace(absoluteFileName.lastIndexOf("."), 4, ".pdf");
    else {
        if (absoluteFileName.endsWith(".pdf") == false)
            absoluteFileName.append(".pdf");
    }

    QPrinter* printer = new QPrinter(QPrinter::HighResolution);
    printer->setFontEmbeddingEnabled(true);
    printer->setOutputFileName(absoluteFileName);
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setPaperSize(QPrinter::A4);
    printDocument(printer);
}


Exporter* Exporter::instance() {
    static Exporter instance;
    return &instance;
}


void Exporter::print() {
    QPrintPreviewDialog previewDialog;
    previewDialog.setModal(true);
    connect(&previewDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printDocument(QPrinter*)));
    previewDialog.show();
    previewDialog.exec();
}


void Exporter::printDocument(QPrinter* printer) {
    QTextDocument* document = this->document();
    if (document != 0)
        document->print(printer);
}


void Exporter::setRecipeData(RecipeData& recipeData) {
    m_recipeData = recipeData;
}


QTextDocument* Exporter::document() {
    QTextDocument* document = new QTextDocument();
    QTextCursor cursor(document);

    /*
     * setup fonts
     */

    QString family;
    if (QSettings().value("format/font/default").toBool() == true) {
        int id = QFontDatabase::addApplicationFont(":/font/gentium");
        family = QFontDatabase::applicationFontFamilies(id).at(0);
    } else {
        family = QSettings().value("format/font/familyName").toString();
    }

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

    QTextBlockFormat centeredFormat;
    centeredFormat.setAlignment(Qt::AlignHCenter);

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

    // headline
    cursor.setBlockFormat(centeredFormat);
    cursor.insertText(m_recipeData.headline(), h1Format);
    cursor.insertBlock(oneHalfSpacingFormat, defaultFormat);

    // ingredients headline
    cursor.insertBlock(doubleSpacingFormat);
    QString ingredientTitle = trUtf8("Ingredients");
    if (QSettings().value("format/paragraphTitles/default").toBool() == false)
        ingredientTitle = QSettings().value("format/paragraphTitles/ingredients").toString();
    cursor.insertText(ingredientTitle, h2Format);
    if (m_recipeData.servingCount().isEmpty() == false) {
        cursor.insertText(" " + m_recipeData.servingCount());
    }

    // ingredient list
    QList<QMap<QString, QString> > ingredients = m_recipeData.ingredients();
    int size = ingredients.size();
    for (int i = 0; i < size; ++i) {
        QMap<QString, QString> entry = ingredients[i];
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
    QString preparationTitle = trUtf8("Preparation");
    if (QSettings().value("format/paragraphTitles/default").toBool() == false)
        preparationTitle = QSettings().value("format/paragraphTitles/preparation").toString();
    cursor.insertText(preparationTitle, h2Format);

    oneHalfSpacingFormat.setBottomMargin(basePointSize);
    cursor.insertBlock(oneHalfSpacingFormat, defaultFormat);

    // preparation steps
    cursor.createList(listFormat);
    QList<QMap<QString, QString> > preparationSteps = m_recipeData.preparationSteps();
    for (int i = 0; i < preparationSteps.size(); ++i) {
        QMap<QString, QString> entry = preparationSteps[i];
        if (entry["type"] == "preparationStep") {
            if (i > 0) {
                cursor.insertBlock();
            }
            cursor.insertText(entry["text"]);
        }
    }

    return document;
}


QString Exporter::xml() {
    QList<QMap<QString, QString> > ingredientList = m_recipeData.ingredients();
    QList<QMap<QString, QString> > preparationStepList = m_recipeData.preparationSteps();

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
    for (QList<QMap<QString, QString> >::Iterator it = ingredientList.begin(); it != ingredientList.end(); ++it) {
        QMap<QString, QString> entry = *it;
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
    for (QList<QMap<QString, QString> >::Iterator it = preparationStepList.begin(); it != preparationStepList.end(); ++it) {
        QMap<QString, QString> entry = *it;
        stream.writeTextElement("step", entry["text"]);
    }
    stream.writeEndElement();

    // document end
    stream.writeEndElement();
    stream.writeEndDocument();

    return xml;
}
