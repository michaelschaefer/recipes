#include <QPrinter>
#include <QPrintDialog>
#include <QTextList>
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

    double basePointSize = font().pointSize();
    QFont defaultFont, h1, h2;

    defaultFont.setWeight(QFont::Normal);
    defaultFont.setPointSize(basePointSize);

    h1.setWeight(QFont::Bold);
    h1.setPointSize(basePointSize * 1.6);

    h2.setWeight(QFont::Bold);
    h2.setPointSize(basePointSize);


    /*
     * formats
     */

    QTextCharFormat defaultFormat;
    defaultFormat.setFont(defaultFont);

    QTextCharFormat h1Format;
    h1Format.setFont(h1);

    QTextCharFormat h2Format;
    h2Format.setFont(h2);

    QTextListFormat listFormat;
    listFormat.setStyle(QTextListFormat::ListDecimal);


    /*
     * recipe
     */

    QTextCursor cursor(textCursor());
    
    // headline
    setAlignment(Qt::AlignHCenter);
    cursor.insertText(m_recipeData.headline(), h1Format);

    cursor.insertBlock(QTextBlockFormat(), h2Format);
    cursor.insertBlock();

    // ingredients
    cursor.insertText(trUtf8("Ingredients"), h2Format);
    if (m_recipeData.servingCount().isEmpty() == false) {
        cursor.insertText(" " + m_recipeData.servingCount());
    }

    bool lastEntryWasSection = false;
    QList<QMap<QString, QString> > ingredients = m_recipeData.ingredients();
    for (int i = 0; i < ingredients.size(); ++i) {
        QMap<QString, QString> entry = ingredients[i];
        if (entry["type"] == "section") {
            cursor.insertBlock();
            cursor.insertBlock();
            cursor.insertText(entry["title"], h2Format);
            lastEntryWasSection = true;
        } else if (entry["type"] == "ingredient") {
            cursor.insertBlock();
            if (lastEntryWasSection == true) {
                cursor.insertBlock();
                lastEntryWasSection = false;
            }
            cursor.insertText(entry["amount"] + entry["unit"] + "\t" + entry["name"], defaultFormat);
        }
    }

    cursor.insertBlock();
    cursor.insertBlock();
    cursor.insertBlock();

    // preparation
    cursor.insertText(trUtf8("Preparation"), h2Format);
    cursor.insertBlock();
    cursor.insertBlock();
    cursor.createList(listFormat);

    QList<QMap<QString, QString> > preparationSteps = m_recipeData.preparationSteps();
    for (int i = 0; i < preparationSteps.size(); ++i) {
        QMap<QString, QString> entry = preparationSteps[i];
        if (entry["type"] == "preparationStep") {
            if (i > 0) {
                cursor.insertBlock();
            }
            cursor.insertText(entry["text"], defaultFormat);
        }
    }
}


void ExportWidget::print() {
    QPrinter printer;
    QPrintDialog dialog(&printer, m_parent);
    if (dialog.exec() == QDialog::Accepted) {
        document()->print(&printer);
    }
}
