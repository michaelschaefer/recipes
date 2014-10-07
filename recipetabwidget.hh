#ifndef RECIPETABWIDGET_HH
#define RECIPETABWIDGET_HH


#include <QList>
#include <QTabWidget>
#include "recipeedit.hh"


class RecipeTabWidget : public QTabWidget {

    Q_OBJECT


signals:

    void empty(bool);


public:

    RecipeTabWidget(QWidget* parent = 0);    


public slots:

    void closeAllTabs();
    bool closeCurrentTab();
    void keyPressEvent(QKeyEvent* event);
    void newRecipe();
    void openRecipe();
    void recipeChanged(RecipeEdit* recipeEdit);
    void saveAllTabs();
    void updateTabText(RecipeEdit* recipeEdit);


private slots:

    bool closeTab(int index);


private:

    void addRecipe(RecipeEdit* recipeEdit);    
    RecipeEdit* currentRecipe();
    void updateTabText(int index);

    QList<RecipeEdit*> m_recipes;

};

#endif // RECIPETABWIDGET_HH
