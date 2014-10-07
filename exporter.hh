#ifndef EXPORTER_HH
#define EXPORTER_HH

#include <QPrinter>
#include <QTextEdit>
#include "recipedata.hh"


class Exporter : public QObject {

    Q_OBJECT


public:

    typedef QMap<QString, QString> EntryType;
    typedef QList<EntryType> EntryListType;


    Exporter(RecipeData recipeData, QWidget* parent = 0);
    void print(bool asPdf = false);
    QTextEdit* textEdit();
    QString xml();



private slots:

    void printDocument(QPrinter* printer);


private:

    QWidget* m_parent;
    RecipeData m_recipeData;

};

#endif // EXPORTER_HH
