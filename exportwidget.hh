#ifndef EXPORTWIDGET_HH
#define EXPORTWIDGET_HH


#include <QTextEdit>
#include "recipedata.hh"


class ExportWidget : public QTextEdit {

    Q_OBJECT


public:

    ExportWidget(RecipeData recipeData, QWidget* parent = 0);
    void print();


private:

    RecipeData m_recipeData;
    QWidget* m_parent;

};

#endif // EXPORTWIDGET_HH
