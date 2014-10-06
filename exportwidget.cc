#include <QFileDialog>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QTextList>
#include <QTextTable>
#include "exportwidget.hh"


ExportWidget::ExportWidget(RecipeData recipeData, QWidget* parent)
    : QTextEdit(parent)
{
    m_parent = parent;
    m_recipeData = recipeData;
    setReadOnly(true);

    /*
     * setup fonts
     */

    int id = QFontDatabase::addApplicationFont(":/font/garamond");
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

    QTextCursor cursor(textCursor());    

    // headline        
    setAlignment(Qt::AlignHCenter);
    setFont(defaultFont);
    cursor.insertText(m_recipeData.headline(), h1Format);
    cursor.insertBlock(oneHalfSpacingFormat, defaultFormat);

    // ingredients headline
    cursor.insertBlock(doubleSpacingFormat);
    cursor.insertText(trUtf8("Ingredients"), h2Format);
    if (m_recipeData.servingCount().isEmpty() == false) {
        cursor.insertText(" " + m_recipeData.servingCount());
    }

    // ingredient list
    QList<QMap<QString, QString> > ingredients = m_recipeData.ingredients();
    int size = ingredients.size();
    for (int i = 0; i < size; ++i) {
        QMap<QString, QString> entry = ingredients[i];        
        if (i == 0 || (i < size-1 && ingredients[i+1]["type"] == "section")) {
            cursor.insertBlock(doubleSpacingFormat);
        } else {
            cursor.insertBlock(oneHalfSpacingFormat);
        }

        if (entry["type"] == "section") {
            cursor.insertText(entry["title"], h2Format);
        } else if (entry["type"] == "ingredient") {
            cursor.insertText(entry["amount"] + entry["unit"] + "\t" + entry["name"], defaultFormat);
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
}


void ExportWidget::print(bool toPdf) {
    QPrinter* printer = new QPrinter(QPrinter::HighResolution);
    if (toPdf == true) {
        QString title = trUtf8("Export as PDF");
        QString filter = trUtf8("PDF documents (*.pdf)");
        QString filename = QFileDialog::getSaveFileName(m_parent, title, QDir::homePath(), filter);
        if (filename.isEmpty() == false) {
            printer->setOutputFileName(filename);
            printer->setOutputFormat(QPrinter::PdfFormat);
            printer->setPaperSize(QPrinter::A4);
            printDocument(printer);
        }
    } else {
        QPrintPreviewDialog previewDialog(m_parent);
        connect(&previewDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printDocument(QPrinter*)));
        previewDialog.show();
        previewDialog.exec();
    }
}


void ExportWidget::printDocument(QPrinter* printer) {
    document()->print(printer);
}
