#include <QMenuBar>
#include <QMessageBox>
#include "exportwidget.hh"
#include "mainwindow.hh"


QString MainWindow::ApplicationName = "recipes";
QString MainWindow::ApplicationVersion = "0.1";


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {    
    setupMenuFile();
    setupMenuEdit();
    setupMenuHelp();

    m_splitter = new QSplitter(this);
    m_tabWidget = new RecipeTabWidget(m_splitter);
    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeCurrentRecipe()));
    m_splitter->addWidget(m_tabWidget);

    m_currentRecipe = 0;

    setCentralWidget(m_splitter);
    setWindowIcon(QIcon(":/img/recipes"));       
    showMaximized();
}


void MainWindow::about() {
    QString copyright = trUtf8(
                "(c) 2014 Michael Schaefer, "
                "<a href=\"http://www.michael-schaefer.org/en/\">"
                "www.michael-schaefer.org/en/"
                "</a>"
                );
    QString information = trUtf8(
                "A program to create, store, search and print recipes."
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
    m_currentRecipe = (RecipeEdit*)m_tabWidget->currentWidget();
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


void MainWindow::editHeadline() {
    if (m_currentRecipe != 0) {
        m_currentRecipe->editHeadline();
    }
}


void MainWindow::editIngredient() {
    if (m_currentRecipe != 0) {
        m_currentRecipe->addIngredient();
    }
}


void MainWindow::editPreparationStep() {
    if (m_currentRecipe != 0) {
        m_currentRecipe->addPreparationStep();
    }
}


void MainWindow::editSection() {
    if (m_currentRecipe != 0) {
        m_currentRecipe->addSection();
    }
}


void MainWindow::editServingCount() {
    if (m_currentRecipe != 0) {
        m_currentRecipe->editServingCount();
    }
}


void MainWindow::fileClose() {
    m_tabWidget->closeCurrentTab();
}


void MainWindow::fileCloseAll() {
    m_tabWidget->closeAllTabs();
}


void MainWindow::fileExport() {
    if (m_currentRecipe != 0) {
        ExportWidget exportWidget(m_currentRecipe->recipeData(), this);
        exportWidget.print(true);
    }
}


void MainWindow::fileNew() {    
    m_tabWidget->newRecipe();
}


void MainWindow::fileOpen() {
    m_tabWidget->openRecipe();
}


void MainWindow::filePrint() {
    if (m_currentRecipe != 0) {
        ExportWidget exportWidget(m_currentRecipe->recipeData(), this);
        exportWidget.print();
    }
}


void MainWindow::fileSave() {
    m_tabWidget->saveCurrentTab();
}


void MainWindow::fileSaveAll() {
    m_tabWidget->saveAllTabs();
}


void MainWindow::fileSaveAs() {
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

    connect(m_actionHeadline, SIGNAL(triggered()), this, SLOT(editHeadline()));
    connect(m_actionIngredient, SIGNAL(triggered()), this, SLOT(editIngredient()));
    connect(m_actionPreparationStep, SIGNAL(triggered()), this, SLOT(editPreparationStep()));
    connect(m_actionSection, SIGNAL(triggered()), this, SLOT(editSection()));
    connect(m_actionServingCount, SIGNAL(triggered()), this, SLOT(editServingCount()));

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

    m_actionClose = new QAction(trUtf8("Close"), m_menuFile);
    m_actionCloseAll = new QAction(trUtf8("Close all"), m_menuFile);
    m_actionExport = new QAction(trUtf8("&Export as PDF"), m_menuFile);
    m_actionNew = new QAction(trUtf8("&New"), m_menuFile);
    m_actionOpen = new QAction(trUtf8("&Open"), m_menuFile);
    m_actionPrint = new QAction(trUtf8("&Print"), m_menuFile);
    m_actionQuit = new QAction(trUtf8("&Quit"), m_menuFile);
    m_actionSave = new QAction(trUtf8("&Save"), m_menuFile);    
    m_actionSaveAll = new QAction(trUtf8("Save all"), m_menuFile);
    m_actionSaveAs = new QAction(trUtf8("Save as"), m_menuFile);

    m_actionClose->setShortcut(Qt::CTRL + Qt::Key_W);
    m_actionCloseAll->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_W);    
    m_actionNew->setShortcut(Qt::CTRL + Qt::Key_N);
    m_actionOpen->setShortcut(Qt::CTRL + Qt::Key_O);
    m_actionPrint->setShortcut(Qt::CTRL + Qt::Key_P);
    m_actionQuit->setShortcut(Qt::CTRL + Qt::Key_Q);        
    m_actionSave->setShortcut(Qt::CTRL + Qt::Key_S);
    m_actionSaveAll->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);

    connect(m_actionClose, SIGNAL(triggered()), this, SLOT(fileClose()));
    connect(m_actionCloseAll, SIGNAL(triggered()), this, SLOT(fileCloseAll()));
    connect(m_actionExport, SIGNAL(triggered()), this, SLOT(fileExport()));
    connect(m_actionNew, SIGNAL(triggered()), this, SLOT(fileNew()));
    connect(m_actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(m_actionPrint, SIGNAL(triggered()), this, SLOT(filePrint()));
    connect(m_actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));    
    connect(m_actionSaveAll, SIGNAL(triggered()), this, SLOT(fileSaveAll()));
    connect(m_actionSaveAs, SIGNAL(triggered()), this, SLOT(fileSaveAs()));

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

    m_actionAboutQt = new QAction(trUtf8("About Qt"), m_menuHelp);
    m_actionAbout = new QAction(trUtf8("About") + " " + MainWindow::ApplicationName, m_menuHelp);

    connect(m_actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    connect(m_actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    m_menuHelp->addAction(m_actionAboutQt);
    m_menuHelp->addAction(m_actionAbout);

    menuBar()->addMenu(m_menuHelp);
}
