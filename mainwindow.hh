#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH


#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMenu>
#include <QSplitter>
#include "recipeedit.hh"
#include "recipetabwidget.hh"


class MainWindow : public QMainWindow {

    Q_OBJECT


public:

    MainWindow(QWidget* parent = 0);

    static QString ApplicationName;
    static QString ApplicationVersion;


public slots:

    void about();
    void aboutQt();


private slots:

    void changeCurrentRecipe();
    void closeEvent(QCloseEvent* event);    
    void setActionInvisibility(bool invisible);


private:

    void connectRecipe(RecipeEdit* recipeEdit);
    void disconnectRecipe(RecipeEdit* recipeEdit);
    void setupMenuEdit();
    void setupMenuFile();
    void setupMenuHelp();


    QAction* m_actionAbout;
    QAction* m_actionAboutQt;
    QAction* m_actionClose;
    QAction* m_actionCloseAll;
    QAction* m_actionExport;
    QAction* m_actionHeadline;
    QAction* m_actionIngredient;
    QAction* m_actionNew;
    QAction* m_actionOpen;
    QAction* m_actionPreparationStep;
    QAction* m_actionPreview;
    QAction* m_actionPrint;
    QAction* m_actionQuit;
    QAction* m_actionSave;
    QAction* m_actionSaveAs;
    QAction* m_actionSaveAll;
    QAction* m_actionSection;
    QAction* m_actionServingCount;
    QMenu* m_menuEdit;
    QMenu* m_menuEditIngredients;
    QMenu* m_menuFile;
    QMenu* m_menuHelp;
    QSplitter* m_splitter;
    RecipeEdit* m_currentRecipe;
    RecipeTabWidget* m_tabWidget;

};

#endif // MAINWINDOW_HH
