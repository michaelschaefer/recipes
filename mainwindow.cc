#include <QDebug>
#include <QFontDatabase>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolButton>
#include "exporter.hh"
#include "librarythread.hh"
#include "mainwindow.hh"


QString MainWindow::ApplicationName = "recipes";
QString MainWindow::ApplicationVersion = "0.2";


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    m_splitter = new QSplitter(this);
    m_searchWidget = new SearchWidget(m_splitter);    
    m_tabWidget = new RecipeTabWidget(m_splitter);

    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeCurrentRecipe()));
    connect(m_tabWidget, SIGNAL(empty(bool)), this, SLOT(setActionInvisibility(bool)));
    connect(m_searchWidget, SIGNAL(recipeSelected(QString)), m_tabWidget, SLOT(openRecipe(QString)));    

    m_splitter->addWidget(m_tabWidget);
    m_splitter->addWidget(m_searchWidget);

    m_currentRecipe = 0;

    setCentralWidget(m_splitter);
    setWindowIcon(QIcon(":/img/recipes"));       
    showMaximized();

    setupMenuEdit();
    setupMenuFile();    
    setupMenuHelp();
    setupMenuLibrary();

    menuBar()->addMenu(m_menuFile);
    menuBar()->addMenu(m_menuEdit);
    menuBar()->addMenu(m_menuLibrary);
    menuBar()->addMenu(m_menuHelp);

    setupToolBar();
    setActionInvisibility(true);

    m_statusBarLabel = new QLabel(statusBar());
    statusBar()->addWidget(m_statusBarLabel);

    m_library = Library::instance();
    connect(m_library, SIGNAL(statusBarMessage(QString)), this, SLOT(showStatusBarMessage(QString)));
    connect(m_library, SIGNAL(updated()), m_searchWidget, SLOT(updateData()));
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
    connect(m_actionPreview, SIGNAL(triggered(bool)), recipeEdit, SLOT(togglePreview(bool)));
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
    disconnect(m_actionPreview, SIGNAL(triggered(bool)), recipeEdit, SLOT(togglePreview(bool)));
}


void MainWindow::fileSettings() {
    SettingsDialog settings(this);
    settings.setWindowTitle(trUtf8("Settings"));
    settings.show();
    settings.exec();
}


void MainWindow::helpAbout() {
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


void MainWindow::helpAboutQt() {
    QMessageBox::aboutQt(this);
}


void MainWindow::libraryManagePaths() {
    QString title = trUtf8("Manage paths");
    LibraryPathDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setPathList(m_library->getPathList());
    dialog.show();

    if (dialog.exec() == QDialog::Accepted) {
        QStringList pathList = dialog.getPathList();
        setMenuLibraryEnabled(false);
        LibraryThread* thread = new LibraryThread(m_library, LibraryThread::SetPathList, this);
        thread->setPathList(pathList);
        connect(thread, SIGNAL(finished()), this, SLOT(setMenuLibraryEnabled()));
        thread->start();
    }
}


void MainWindow::libraryRebuild() {
    setMenuLibraryEnabled(false);
    LibraryThread* thread = new LibraryThread(m_library, LibraryThread::Rebuild, this);
    connect(thread, SIGNAL(finished()), this, SLOT(setMenuLibraryEnabled()));
    thread->start();
}


void MainWindow::libraryUpdate() {
    setMenuLibraryEnabled(false);
    LibraryThread* thread = new LibraryThread(m_library, LibraryThread::Update, this);
    connect(thread, SIGNAL(finished()), this, SLOT(setMenuLibraryEnabled()));
    thread->start();
}


void MainWindow::setActionInvisibility(bool invisible) {
    m_actionClose->setEnabled(!invisible);
    m_actionCloseAll->setEnabled(!invisible);
    m_actionExport->setEnabled(!invisible);
    m_actionPreview->setEnabled(!invisible);
    m_actionPrint->setEnabled(!invisible);
    m_actionSave->setEnabled(!invisible);
    m_actionSaveAll->setEnabled(!invisible);
    m_actionSaveAs->setEnabled(!invisible);
    m_menuEdit->setEnabled(!invisible);

    m_actionToolBarClose->setEnabled(!invisible);
    m_actionToolBarExport->setEnabled(!invisible);
    m_actionToolBarPrint->setEnabled(!invisible);
    m_actionToolBarSave->setEnabled(!invisible);
    m_actionToolBarHeadline->setEnabled(!invisible);
    m_actionToolButtonIngredient->setEnabled(!invisible);
    m_actionToolBarPreparationStep->setEnabled(!invisible);
    m_actionToolBarPreview->setEnabled(!invisible);
}


void MainWindow::setupMenuEdit() {
    m_menuEdit = new QMenu(trUtf8("&Edit"), menuBar());
    m_menuEditIngredients = new QMenu(trUtf8("&Ingredients"), m_menuEdit);

    m_actionHeadline = new QAction(trUtf8("Edit &headline"), m_menuEdit);
    m_actionIngredient = new QAction(trUtf8("Add &ingredient"), m_menuEditIngredients);
    m_actionPreparationStep = new QAction(trUtf8("Add &preparation step"), m_menuEdit);
    m_actionPreview = new QAction(trUtf8("Show p&review"), m_menuEdit);
    m_actionSection = new QAction(trUtf8("Add &section"), m_menuEditIngredients);
    m_actionServingCount = new QAction(trUtf8("Edit serving &count"), m_menuEditIngredients);

    m_actionPreview->setCheckable(true);
    m_actionPreview->setChecked(false);

    m_actionHeadline->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E, Qt::Key_H));
    m_actionIngredient->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E, Qt::Key_I));
    m_actionPreparationStep->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E, Qt::Key_P));
    m_actionPreview->setShortcut(Qt::Key_F4);
    m_actionSection->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E, Qt::Key_S));
    m_actionServingCount->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E, Qt::Key_C));

    m_menuEditIngredients->addAction(m_actionServingCount);
    m_menuEditIngredients->addAction(m_actionIngredient);
    m_menuEditIngredients->addAction(m_actionSection);

    m_menuEdit->addAction(m_actionHeadline);
    m_menuEdit->addMenu(m_menuEditIngredients);
    m_menuEdit->addAction(m_actionPreparationStep);
    m_menuEdit->addAction(m_actionPreview);    
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
    m_actionSettings = new QAction(trUtf8("Se&ttings"), m_menuFile);

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
    connect(m_actionSettings, SIGNAL(triggered()), this, SLOT(fileSettings()));

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
    m_menuFile->addAction(m_actionSettings);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_actionQuit);        
}


void MainWindow::setupMenuHelp() {    
    m_menuHelp = new QMenu(trUtf8("&Help"), menuBar());

    m_actionAboutQt = new QAction(trUtf8("About &Qt"), m_menuHelp);
    m_actionAbout = new QAction(trUtf8("&About") + " " + MainWindow::ApplicationName, m_menuHelp);

    connect(m_actionAboutQt, SIGNAL(triggered()), this, SLOT(helpAboutQt()));
    connect(m_actionAbout, SIGNAL(triggered()), this, SLOT(helpAbout()));

    m_menuHelp->addAction(m_actionAboutQt);
    m_menuHelp->addAction(m_actionAbout);    
}


void MainWindow::setupMenuLibrary() {
    m_menuLibrary = new QMenu(trUtf8("&Library"), menuBar());

    m_actionManagePaths = new QAction(trUtf8("Manager &paths"), m_menuLibrary);
    m_actionRebuild = new QAction(trUtf8("&Rebuild"), m_menuLibrary);
    m_actionSearch = new QAction(trUtf8("&Search"), m_menuLibrary);
    m_actionUpdate = new QAction(trUtf8("&Update"), m_menuLibrary);

    m_actionManagePaths->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L, Qt::Key_P));
    m_actionRebuild->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L, Qt::Key_R));
    m_actionSearch->setShortcut(Qt::Key_F2);
    m_actionUpdate->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L, Qt::Key_U));

    connect(m_actionManagePaths, SIGNAL(triggered()), this, SLOT(libraryManagePaths()));
    connect(m_actionRebuild, SIGNAL(triggered()), this, SLOT(libraryRebuild()));
    connect(m_actionSearch, SIGNAL(triggered()), m_searchWidget, SLOT(toggleVisibility()));
    connect(m_actionUpdate, SIGNAL(triggered()), this, SLOT(libraryUpdate()));    

    m_menuLibrary->addAction(m_actionManagePaths);
    m_menuLibrary->addAction(m_actionRebuild);
    m_menuLibrary->addAction(m_actionUpdate);
    m_menuLibrary->addSeparator();
    m_menuLibrary->addAction(m_actionSearch);
}


void MainWindow::setMenuLibraryEnabled(bool enabled) {
    m_menuLibrary->setEnabled(enabled);
}


void MainWindow::setupToolBar() {
    int id = QFontDatabase::addApplicationFont(":/font/fontawesome");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);

    QFont toolBarFont(family);
    toolBarFont.setPointSize(font().pointSize() * 2);

    m_toolBar = new QToolBar(this);
    m_toolBar->setMovable(false);
    addToolBar(m_toolBar);
    setContextMenuPolicy(Qt::NoContextMenu);

    m_actionToolBarClose = new QAction("\uF00D", m_toolBar);
    m_actionToolBarExport = new QAction("\uF1C1", m_toolBar);
    m_actionToolBarHeadline = new QAction("\uF1DC", m_toolBar);
    m_actionToolBarNew = new QAction("\uF0F6", m_toolBar);
    m_actionToolBarOpen = new QAction("\uF115", m_toolBar);
    m_actionToolBarPreparationStep = new QAction("\uF0F5", m_toolBar);
    m_actionToolBarPreview = new QAction("\uF06E", m_toolBar);
    m_actionToolBarPrint = new QAction("\uF02F", m_toolBar);
    m_actionToolBarSave = new QAction("\uF0C7", m_toolBar);
    m_actionToolBarSearch = new QAction("\uF002", m_toolBar);

    m_actionToolBarClose->setFont(toolBarFont);
    m_actionToolBarExport->setFont(toolBarFont);
    m_actionToolBarHeadline->setFont(toolBarFont);
    m_actionToolBarNew->setFont(toolBarFont);
    m_actionToolBarOpen->setFont(toolBarFont);
    m_actionToolBarPrint->setFont(toolBarFont);
    m_actionToolBarPreparationStep->setFont(toolBarFont);
    m_actionToolBarPreview->setFont(toolBarFont);
    m_actionToolBarSave->setFont(toolBarFont);
    m_actionToolBarSearch->setFont(toolBarFont);

    m_actionToolBarClose->setToolTip(trUtf8("Close"));
    m_actionToolBarExport->setToolTip(trUtf8("Export as PDF"));
    m_actionToolBarHeadline->setToolTip(trUtf8("Edit headline"));
    m_actionToolBarNew->setToolTip(trUtf8("New"));
    m_actionToolBarOpen->setToolTip(trUtf8("Open"));
    m_actionToolBarPrint->setToolTip(trUtf8("Print"));
    m_actionToolBarPreparationStep->setToolTip(trUtf8("Add preparation step"));
    m_actionToolBarPreview->setToolTip(trUtf8("Toggle preview"));
    m_actionToolBarSave->setToolTip(trUtf8("Save"));
    m_actionToolBarSearch->setToolTip(trUtf8("Search library"));

    m_actionToolButtonIngredient = new QToolButton(m_toolBar);
    m_actionToolButtonIngredient->setFont(toolBarFont);
    m_actionToolButtonIngredient->setText("\uF094");
    m_actionToolButtonIngredient->setToolTip(trUtf8("Ingredients"));
    m_actionToolButtonIngredient->setPopupMode(QToolButton::InstantPopup);

    m_menuToolButtonIngredient = new QMenu(m_actionToolButtonIngredient);
    m_menuToolButtonIngredient->addAction(m_actionServingCount);
    m_menuToolButtonIngredient->addAction(m_actionIngredient);
    m_menuToolButtonIngredient->addAction(m_actionSection);
    m_actionToolButtonIngredient->setMenu(m_menuToolButtonIngredient);

    connect(m_actionToolBarClose, SIGNAL(triggered()), m_actionClose, SLOT(trigger()));
    connect(m_actionToolBarExport, SIGNAL(triggered()), m_actionExport, SLOT(trigger()));
    connect(m_actionToolBarHeadline, SIGNAL(triggered()), m_actionHeadline, SLOT(trigger()));
    connect(m_actionToolBarNew, SIGNAL(triggered()), m_actionNew, SLOT(trigger()));
    connect(m_actionToolBarOpen, SIGNAL(triggered()), m_actionOpen, SLOT(trigger()));
    connect(m_actionToolBarPrint, SIGNAL(triggered()), m_actionPrint, SLOT(trigger()));
    connect(m_actionToolBarPreparationStep, SIGNAL(triggered()), m_actionPreparationStep, SLOT(trigger()));
    connect(m_actionToolBarPreview, SIGNAL(triggered()), m_actionPreview, SLOT(trigger()));
    connect(m_actionToolBarSave, SIGNAL(triggered()), m_actionSave, SLOT(trigger()));
    connect(m_actionToolBarSearch, SIGNAL(triggered()), m_actionSearch, SLOT(trigger()));

    m_toolBar->addAction(m_actionToolBarNew);
    m_toolBar->addAction(m_actionToolBarOpen);
    m_toolBar->addAction(m_actionToolBarSearch);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actionToolBarSave);
    m_toolBar->addAction(m_actionToolBarClose);
    m_toolBar->addAction(m_actionToolBarExport);
    m_toolBar->addAction(m_actionToolBarPrint);
    m_toolBar->addAction(m_actionToolBarPreview);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actionToolBarHeadline);
    m_toolBar->addWidget(m_actionToolButtonIngredient);
    m_toolBar->addAction(m_actionToolBarPreparationStep);    
}


void MainWindow::showStatusBarMessage(QString message) {
    m_statusBarLabel->setText(message);
}
