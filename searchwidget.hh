#ifndef SEARCHWIDGET_HH
#define SEARCHWIDGET_HH


#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>
#include "library.hh"
#include "searchfiltermodel.hh"
#include "searchfilterwidget.hh"


class SearchWidget : public QWidget {

    Q_OBJECT


signals:

    void recipeSelected(QString);


public:

    SearchWidget(QSplitter* parent = 0);

    void keyPressEvent(QKeyEvent* event);


public slots:

    void toggleVisibility();
    void updateData();


private slots:

    void openRecipe();
    void resetLabelText();
    void selectRecipe(bool open = false);


private:

    Library* m_library;
    QVBoxLayout* m_layout;
    QLabel* m_labelRecipeInfoHeader;
    QLabel* m_labelRecipeInfoHeadline;
    QLabel* m_labelRecipeInfoPath;    
    QListView* m_listViewRecipes;    
    QSplitter* m_parent;
    SearchFilterModel* m_searchFilterModel;
    SearchFilterWidget* m_searchFilterWidget;


};

#endif // SEARCHWIDGET_HH
