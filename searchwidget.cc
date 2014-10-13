#include "searchwidget.hh"


SearchWidget::SearchWidget(QSplitter* parent) : QWidget() {
    m_parent = parent;
    m_library = Library::instance();

    m_gridLayout = new QGridLayout();    
    setLayout(m_gridLayout);

    m_buttonOpen = new QPushButton(trUtf8("Open"));
    m_editSearchQuery = new QLineEdit(this);
    m_editSearchQuery->setPlaceholderText(trUtf8("Enter search query"));

    m_labelRecipeInfoHeader = new QLabel(trUtf8("<b>Recipe information</b>"), this);
    m_labelRecipeInfoHeadline = new QLabel(trUtf8("Headline: ") + trUtf8("no recipe selected"), this);
    m_labelRecipeInfoPath = new QLabel(trUtf8("Path: ") + trUtf8("no recipe selected"), this);

    m_listViewModelRecipes = new SearchFilterModel(this);
    m_listViewModelRecipes->setFilterKeyColumn(0);
    m_listViewModelRecipes->setFilterCaseSensitivity(Qt::CaseInsensitive);
    updateData();

    m_listViewRecipes = new QListView();
    m_listViewRecipes->setModel(m_listViewModelRecipes);

    m_gridLayout->addWidget(m_editSearchQuery, 0, 0);
    m_gridLayout->addWidget(m_buttonOpen, 0, 1);
    m_gridLayout->addWidget(m_listViewRecipes, 1, 0, 1, 2);
    m_gridLayout->addWidget(m_labelRecipeInfoHeader, 2, 0, 1, 2);
    m_gridLayout->addWidget(m_labelRecipeInfoHeadline, 3, 0, 1, 2);
    m_gridLayout->addWidget(m_labelRecipeInfoPath, 4, 0, 1, 2);

    connect(m_listViewRecipes->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(selectRecipe()));
    connect(m_listViewRecipes, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openRecipe()));
    connect(m_buttonOpen, SIGNAL(clicked()), this, SLOT(openRecipe()));
    connect(m_editSearchQuery, SIGNAL(textChanged(QString)), this, SLOT(filter(QString)));

    setVisible(false);
}


void SearchWidget::filter(QString pattern) {
    m_listViewModelRecipes->setFilterFixedString(pattern);
}


void SearchWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape)
        setVisible(false);
    event->accept();
}


void SearchWidget::openRecipe() {
    selectRecipe(true);
}


void SearchWidget::selectRecipe(bool open) {
    QModelIndex index = m_listViewRecipes->currentIndex();

    Database::File fileInfo;
    Database::Path pathInfo;
    int fileId = m_listViewModelRecipes->data(index, Qt::UserRole).toInt();
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
        m_editSearchQuery->setFocus();
    }
}


void SearchWidget::updateData() {
    m_listViewModelRecipes->clearItems();
    foreach (const Database::RecipeType& pair, m_library->getRecipeList()) {
        m_listViewModelRecipes->insertItem(pair);
    }
}
