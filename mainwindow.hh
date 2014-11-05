#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH


#include <QMainWindow>
#include <QToolBar>
#include "recipeedit.hh"
#include "recipetabwidget.hh"
#include "searchwidget.hh"
#include "settingsdialog.hh"


class MainWindow : public QMainWindow {

    Q_OBJECT


public:

    MainWindow(QWidget* parent = 0);

    static QString ApplicationName;
    static QString ApplicationOrganization;
    static QString ApplicationVersion;


public slots:

    void helpAbout();
    void helpAboutQt();


private slots:

    void changeCurrentRecipe();
    void closeEvent(QCloseEvent* event);
    void errorMessage(QString title, QString text);
    void fileSettings();
    void libraryExport();
    void libraryRebuild();
    void libraryUpdate();
    void setEditActionInvisibility(bool invisible);
    void setMenuLibraryEnabled(bool enabled = true);
    void showStatusBarMessage(QString message);


private:

    void connectRecipe(RecipeEdit* recipeEdit);
    void disconnectRecipe(RecipeEdit* recipeEdit);
    void setupMenuEdit();
    void setupMenuFile();
    void setupMenuHelp();
    void setupMenuLibrary();
    void setupToolBar();


    Library* m_library;
    QAction* m_actionAbout;
    QAction* m_actionAboutQt;
    QAction* m_actionBrowse;
    QAction* m_actionClose;
    QAction* m_actionCloseAll;
    QAction* m_actionExport;
    QAction* m_actionExportLibrary;
    QAction* m_actionHeadline;
    QAction* m_actionIngredient;
    QAction* m_actionNew;
    QAction* m_actionOpen;
    QAction* m_actionPreparationStep;
    QAction* m_actionPreview;
    QAction* m_actionPrint;
    QAction* m_actionQuit;
    QAction* m_actionRebuild;
    QAction* m_actionSave;
    QAction* m_actionSaveAs;
    QAction* m_actionSaveAll;
    QAction* m_actionSection;
    QAction* m_actionServingCount;
    QAction* m_actionSettings;
    QAction* m_actionSynchronize;
    QAction* m_actionToolBarBrowse;
    QAction* m_actionToolBarClose;
    QAction* m_actionToolBarExport;
    QAction* m_actionToolBarHeadline;
    QAction* m_actionToolBarNew;
    QAction* m_actionToolBarOpen;
    QAction* m_actionToolBarPreparationStep;
    QAction* m_actionToolBarPreview;
    QAction* m_actionToolBarPrint;
    QAction* m_actionToolBarSave;
    QAction* m_actionUpdate;
    QLabel* m_statusBarLabel;
    QMenu* m_menuEdit;
    QMenu* m_menuEditIngredients;
    QMenu* m_menuFile;
    QMenu* m_menuHelp;
    QMenu* m_menuLibrary;
    QMenu* m_menuToolButtonIngredient;
    SettingsManager* m_settingsManager;
    QSplitter* m_splitter;
    QToolBar* m_toolBar;
    QToolButton* m_actionToolButtonIngredient;
    RecipeEdit* m_currentRecipe;
    RecipeTabWidget* m_tabWidget;
    SearchWidget* m_searchWidget;
    SettingsDialog* m_settingsDialog;

};

#endif // MAINWINDOW_HH
