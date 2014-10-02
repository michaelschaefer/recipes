#ifndef RECIPETABWIDGET_HH
#define RECIPETABWIDGET_HH


#include <QList>
#include <QTabWidget>
#include "recipeedit.hh"


class RecipeTabWidget : public QTabWidget {

    Q_OBJECT


public:

    RecipeTabWidget(QWidget* parent = 0);

    void newRecipe();    
    void openRecipe();
    void updateTabTexts();


public slots:

    void closeAllTabs();
    bool closeCurrentTab();
    void recipeChanged(RecipeEdit* recipeEdit);
    bool saveCurrentTab();
    void saveAllTabs();


private:

    void addRecipe(RecipeEdit* recipeEdit);
    bool closeTab(int index);

    QList<RecipeEdit*> m_recipes;

};

#endif // RECIPETABWIDGET_HH
