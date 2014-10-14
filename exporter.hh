#ifndef EXPORTER_HH
#define EXPORTER_HH

#include <QDir>
#include <QPrinter>
#include <QTextEdit>
#include "recipedata.hh"


class Exporter : public QObject {

    Q_OBJECT


public:

    typedef QMap<QString, QString> EntryType;
    typedef QList<EntryType> EntryListType;


    static Exporter* instance();

    void exportAsPdf(QString dir = QDir::homePath());
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
