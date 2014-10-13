#ifndef SEARCHWIDGET_HH
#define SEARCHWIDGET_HH


#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QSplitter>
#include "library.hh"
#include "searchfiltermodel.hh"


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

    void filter(QString pattern);
    void openRecipe();
    void selectRecipe(bool open = false);


private:

    Library* m_library;
    QGridLayout* m_gridLayout;
    QLabel* m_labelRecipeInfoHeader;
    QLabel* m_labelRecipeInfoHeadline;
    QLabel* m_labelRecipeInfoPath;
    QLineEdit* m_editSearchQuery;
    QListView* m_listViewRecipes;
    QPushButton* m_buttonOpen;
    QSplitter* m_parent;
    SearchFilterModel* m_listViewModelRecipes;


};

#endif // SEARCHWIDGET_HH
