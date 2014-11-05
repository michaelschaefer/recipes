#ifndef EXPORTER_HH
#define EXPORTER_HH

#include <QPrinter>
#include <QTextDocument>
#include "recipedata.hh"
#include "settingsmanager.hh"


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
    SettingsManager* m_settingsManager;

};

#endif // EXPORTER_HH
