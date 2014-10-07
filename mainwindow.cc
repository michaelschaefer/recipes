#include <QDebug>
#include <QMenuBar>
#include <QMessageBox>
#include "mainwindow.hh"


QString MainWindow::ApplicationName = "recipes";
QString MainWindow::ApplicationVersion = "0.1";


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {        
    m_splitter = new QSplitter(this);
    m_tabWidget = new RecipeTabWidget(m_splitter);    
    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeCurrentRecipe()));
    connect(m_tabWidget, SIGNAL(empty(bool)), this, SLOT(setActionInvisibility(bool)));
    m_splitter->addWidget(m_tabWidget);

    m_currentRecipe = 0;

    setCentralWidget(m_splitter);
    setWindowIcon(QIcon(":/img/recipes"));       
    showMaximized();

    setupMenuFile();
    setupMenuEdit();
    setupMenuHelp();
    setActionInvisibility(true);
}


void MainWindow::about() {
    QString copyright = trUtf8(
                "(c) 2014 Michael Schaefer, "
                "<a href=\"http://www.michael-schaefer.org/en/\">"
                "www.michael-schaefer.org/en/"
                "</a>"
                );
    QString information = trUtf8(
                "A program to create, store, export and print recipes."
                );
    QString text = QString("<b>%1 %2</b><br/><br/>%3<br/><br/>%4")
            .arg(MainWindow::ApplicationName)
            .arg(MainWindow::ApplicationVersion)
            .arg(information)
            .arg(copyright);
    QString title = trUtf8("About ") + MainWindow::ApplicationName;

    QMessageBox about(this);
    about.setWindowTitle(title);
    about.setText(text);
    about.setIconPixmap(windowIcon().pixmap(128, 128));
    about.show();
    about.exec();
}


void MainWindow::aboutQt() {
    QMessageBox::aboutQt(this);
}


void MainWindow::changeCurrentRecipe() {
    if (m_currentRecipe != 0)
        disconnectRecipe(m_currentRecipe);

    m_currentRecipe = (RecipeEdit*)m_tabWidget->currentWidget();
    connectRecipe(m_currentRecipe);
}


void MainWindow::closeEvent(QCloseEvent* event) {
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        if (((RecipeEdit*)m_tabWidget->widget(i))->hasUnsavedChanges() == true) {
            QString title = trUtf8("Quit");
            QString text = trUtf8("There are unsaved changes! Are you sure you want to quit?");
            if (QMessageBox::question(this, title, text) == QMessageBox::Yes) {
                event->accept();
                break;
            } else {
                event->ignore();
                break;
            }
        }
    }
}


void MainWindow::connectRecipe(RecipeEdit* recipeEdit) {
    if (recipeEdit == 0)
        return;

    connect(recipeEdit, SIGNAL(saved(RecipeEdit*)), m_tabWidget, SLOT(updateTabText(RecipeEdit*)));
    connect(m_actionHeadline, SIGNAL(triggered()), recipeEdit, SLOT(editHeadline()));
    connect(m_actionIngredient, SIGNAL(triggered()), recipeEdit, SLOT(addIngredient()));
    connect(m_actionPreparationStep, SIGNAL(triggered()), recipeEdit, SLOT(addPreparationStep()));
    connect(m_actionSection, SIGNAL(triggered()), recipeEdit, SLOT(addSection()));
    connect(m_actionServingCount, SIGNAL(triggered()), recipeEdit, SLOT(editServingCount()));
    connect(m_actionExport, SIGNAL(triggered()), recipeEdit, SLOT(exportAsPdf()));
    connect(m_actionPrint, SIGNAL(triggered()), recipeEdit, SLOT(print()));
    connect(m_actionSave, SIGNAL(triggered()), recipeEdit, SLOT(save()));
    connect(m_actionSaveAs, SIGNAL(triggered()), recipeEdit, SLOT(saveAs()));
}


void MainWindow::disconnectRecipe(RecipeEdit* recipeEdit) {
    if (recipeEdit == 0)
        return;

    disconnect(recipeEdit, SIGNAL(saved(RecipeEdit*)), m_tabWidget, SLOT(updateTabText(RecipeEdit*)));
    disconnect(m_actionHeadline, SIGNAL(triggered()), recipeEdit, SLOT(editHeadline()));
    disconnect(m_actionIngredient, SIGNAL(triggered()), recipeEdit, SLOT(addIngredient()));
    disconnect(m_actionPreparationStep, SIGNAL(triggered()), recipeEdit, SLOT(addPreparationStep()));
    disconnect(m_actionSection, SIGNAL(triggered()), recipeEdit, SLOT(addSection()));
    disconnect(m_actionServingCount, SIGNAL(triggered()), recipeEdit, SLOT(editServingCount()));
    disconnect(m_actionExport, SIGNAL(triggered()), recipeEdit, SLOT(exportAsPdf()));
    disconnect(m_actionPrint, SIGNAL(triggered()), recipeEdit, SLOT(print()));
    disconnect(m_actionSave, SIGNAL(triggered()), recipeEdit, SLOT(save()));
    disconnect(m_actionSaveAs, SIGNAL(triggered()), recipeEdit, SLOT(saveAs()));
}


void MainWindow::setActionInvisibility(bool invisible) {
    m_actionClose->setEnabled(!invisible);
    m_actionCloseAll->setEnabled(!invisible);
    m_actionExport->setEnabled(!invisible);
    m_actionPrint->setEnabled(!invisible);
    m_actionSave->setEnabled(!invisible);
    m_actionSaveAll->setEnabled(!invisible);
    m_actionSaveAs->setEnabled(!invisible);
    m_menuEdit->setEnabled(!invisible);
}


void MainWindow::setupMenuEdit() {
    m_menuEdit = new QMenu(trUtf8("&Edit"), menuBar());
    m_menuEditIngredients = new QMenu(trUtf8("&Ingredients"), m_menuEdit);

    m_actionHeadline = new QAction(trUtf8("Edit &headline"), m_menuEdit);
    m_actionIngredient = new QAction(trUtf8("Add &ingredient"), m_menuEditIngredients);
    m_actionPreparationStep = new QAction(trUtf8("Add &preparation step"), m_menuEdit);
    m_actionSection = new QAction(trUtf8("Add &section"), m_menuEditIngredients);
    m_actionServingCount = new QAction(trUtf8("Edit serving &count"), m_menuEditIngredients);

    m_actionHeadline->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E, Qt::Key_H));
    m_actionIngredient->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E, Qt::Key_I));
    m_actionPreparationStep->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E, Qt::Key_P));
    m_actionSection->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E, Qt::Key_S));
    m_actionServingCount->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E, Qt::Key_C));

    m_menuEditIngredients->addAction(m_actionServingCount);
    m_menuEditIngredients->addAction(m_actionIngredient);
    m_menuEditIngredients->addAction(m_actionSection);

    m_menuEdit->addAction(m_actionHeadline);
    m_menuEdit->addMenu(m_menuEditIngredients);
    m_menuEdit->addAction(m_actionPreparationStep);

    menuBar()->addMenu(m_menuEdit);
}


void MainWindow::setupMenuFile() {
    m_menuFile = new QMenu(trUtf8("&File"), menuBar());

    m_actionClose = new QAction(trUtf8("&Close"), m_menuFile);
    m_actionCloseAll = new QAction(trUtf8("C&lose all"), m_menuFile);
    m_actionExport = new QAction(trUtf8("&Export as PDF"), m_menuFile);
    m_actionNew = new QAction(trUtf8("&New"), m_menuFile);
    m_actionOpen = new QAction(trUtf8("&Open"), m_menuFile);
    m_actionPrint = new QAction(trUtf8("&Print"), m_menuFile);
    m_actionQuit = new QAction(trUtf8("&Quit"), m_menuFile);
    m_actionSave = new QAction(trUtf8("&Save"), m_menuFile);    
    m_actionSaveAll = new QAction(trUtf8("S&ave all"), m_menuFile);
    m_actionSaveAs = new QAction(trUtf8("Sa&ve as"), m_menuFile);

    m_actionClose->setShortcut(Qt::CTRL + Qt::Key_W);
    m_actionCloseAll->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_W);    
    m_actionNew->setShortcut(Qt::CTRL + Qt::Key_N);
    m_actionOpen->setShortcut(Qt::CTRL + Qt::Key_O);
    m_actionPrint->setShortcut(Qt::CTRL + Qt::Key_P);
    m_actionQuit->setShortcut(Qt::CTRL + Qt::Key_Q);        
    m_actionSave->setShortcut(Qt::CTRL + Qt::Key_S);
    m_actionSaveAll->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);

    connect(m_actionClose, SIGNAL(triggered()), m_tabWidget, SLOT(closeCurrentTab()));
    connect(m_actionCloseAll, SIGNAL(triggered()), m_tabWidget, SLOT(closeAllTabs()));
    connect(m_actionNew, SIGNAL(triggered()), m_tabWidget, SLOT(newRecipe()));
    connect(m_actionOpen, SIGNAL(triggered()), m_tabWidget, SLOT(openRecipe()));
    connect(m_actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_actionSaveAll, SIGNAL(triggered()), m_tabWidget, SLOT(saveAllTabs()));

    m_menuFile->addAction(m_actionNew);
    m_menuFile->addAction(m_actionOpen);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_actionSave);
    m_menuFile->addAction(m_actionSaveAs);
    m_menuFile->addAction(m_actionSaveAll);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_actionClose);
    m_menuFile->addAction(m_actionCloseAll);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_actionExport);
    m_menuFile->addAction(m_actionPrint);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_actionQuit);    

    menuBar()->addMenu(m_menuFile);
}


void MainWindow::setupMenuHelp() {    
    m_menuHelp = new QMenu(trUtf8("&Help"), menuBar());

    m_actionAboutQt = new QAction(trUtf8("About &Qt"), m_menuHelp);
    m_actionAbout = new QAction(trUtf8("&About") + " " + MainWindow::ApplicationName, m_menuHelp);

    connect(m_actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    connect(m_actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    m_menuHelp->addAction(m_actionAboutQt);
    m_menuHelp->addAction(m_actionAbout);

    menuBar()->addMenu(m_menuHelp);
}
