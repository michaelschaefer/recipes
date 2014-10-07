#ifndef EXPORTER_HH
#define EXPORTER_HH


#include <QObject>
#include <QPrinter>
#include <QTextEdit>
#include "recipedata.hh"


class Exporter : public QObject {

    Q_OBJECT


public:

    Exporter(RecipeData recipeData, QWidget* parent = 0);
    void print(bool asPdf = false);


private slots:

    void printDocument(QPrinter* printer);


private:

    QTextEdit* m_textEdit;
    QWidget* m_parent;

};

#endif // EXPORTER_HH
