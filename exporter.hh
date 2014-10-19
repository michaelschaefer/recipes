#ifndef EXPORTER_HH
#define EXPORTER_HH

#include <QDir>
#include <QPrinter>
#include <QTextEdit>
#include "recipedata.hh"


class Exporter : public QObject {

    Q_OBJECT


public:    

    static Exporter* instance();

    void exportAsPdf(QString fileName = QString(), QString dir = QString());
    void print();
    void setRecipeData(RecipeData& recipeData);
    QTextEdit* textEdit();
    QString xml();



private slots:

    void printDocument(QPrinter* printer);


private:

    Exporter() { m_parent = 0; }
    Exporter(Exporter const&);
    void operator=(Exporter const&);

    QWidget* m_parent;
    RecipeData m_recipeData;

};

#endif // EXPORTER_HH
