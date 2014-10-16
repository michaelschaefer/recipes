#include "searchwidget.hh"


SearchWidget::SearchWidget(QSplitter* parent) : QWidget() {
    m_parent = parent;
    m_library = Library::instance();

    m_layout = new QVBoxLayout();
    setLayout(m_layout);

    m_searchFilterWidget = new SearchFilterWidget(this);

    m_labelRecipeInfoHeader = new QLabel(trUtf8("<b>Recipe information</b>"), this);
    m_labelRecipeInfoHeadline = new QLabel(this);
    m_labelRecipeInfoPath = new QLabel(this);
    resetLabelText();

    m_searchFilterModel = new SearchFilterModel(this);
    m_searchFilterModel->setFilterKeyColumn(0);
    m_searchFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    updateData();

    m_listViewRecipes = new QListView();
    m_listViewRecipes->setModel(m_searchFilterModel);

    m_layout->addWidget(m_searchFilterWidget);
    m_layout->addWidget(m_listViewRecipes, 100);
    m_layout->addWidget(m_labelRecipeInfoHeader);
    m_layout->addWidget(m_labelRecipeInfoHeadline);
    m_layout->addWidget(m_labelRecipeInfoPath);

    connect(m_listViewRecipes->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(selectRecipe()));
    connect(m_listViewRecipes, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openRecipe()));                
    connect(m_searchFilterWidget, SIGNAL(filterUpdated(QString,QList<QList<int> >)), m_searchFilterModel, SLOT(filter(QString,QList<QList<int> >)));

    setVisible(false);
}


void SearchWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape)
        setVisible(false);
    event->accept();
}


void SearchWidget::openRecipe() {
    selectRecipe(true);
}


void SearchWidget::resetLabelText() {
    m_labelRecipeInfoHeadline->setText(trUtf8("Headline: ") + trUtf8("no recipe selected"));
    m_labelRecipeInfoPath->setText(trUtf8("Path: ") + trUtf8("no recipe selected"));
}


void SearchWidget::selectRecipe(bool open) {        
    if (m_listViewRecipes->currentIndex().isValid() == false) {
        resetLabelText();
        return;
    }

    Database::File fileInfo;
    Database::Path pathInfo;
    int fileId = m_searchFilterModel->getFileId(m_listViewRecipes->currentIndex());
    m_library->getFile(fileId, fileInfo);
    m_library->getPath(fileInfo.pathId, pathInfo);

    QString pathName = QString("%1/%2").arg(pathInfo.pathName, fileInfo.fileName);
    if (open == false) {
        m_labelRecipeInfoPath->setText(trUtf8("Path: ") + pathName);
        m_labelRecipeInfoHeadline->setText(trUtf8("Headline: ") + fileInfo.headline);
    } else {        
        emit recipeSelected(pathName);
    }
}


void SearchWidget::toggleVisibility() {
    if (isVisible() == true)
        setVisible(false);
    else {
        if (m_parent != 0) {
            int w = m_parent->width();
            int half = (w - w%2) / 2;
            QList<int> sizeList;
            sizeList.append(half);
            sizeList.append(w - half);
            m_parent->setSizes(sizeList);
        }
        setVisible(true);
        m_searchFilterWidget->setFocus();
    }
}


void SearchWidget::updateData() {
    m_searchFilterModel->clearFilter();
    m_searchFilterModel->clearItems();
    Database::Recipe recipe;
    foreach (recipe, m_library->getRecipeList()) {
        m_searchFilterModel->insertItem(recipe);
    }
}
