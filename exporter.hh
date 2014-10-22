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

    QTextDocument* document();
    void exportAsPdf(QString absoluteFileName = QString(), bool withPrompt = true);
    void print();
    void setRecipeData(RecipeData& recipeData);    
    QString xml();


private slots:

    void printDocument(QPrinter* printer);    


private:

    Exporter();
    Exporter(Exporter const&);
    void operator=(Exporter const&);


    RecipeData m_recipeData;

};

#endif // EXPORTER_HH
