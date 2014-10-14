#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "recipetabwidget.hh"


RecipeTabWidget::RecipeTabWidget(QWidget* parent)
    : QTabWidget(parent)
{    
    setTabsClosable(true);        
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}


void RecipeTabWidget::addRecipe(RecipeEdit* recipeEdit) {
    if (recipeEdit == 0) {
        return;
    }

    connect(recipeEdit, SIGNAL(changed(RecipeEdit*)), this, SLOT(recipeChanged(RecipeEdit*)));
    m_recipes.append(recipeEdit);

    int index = insertTab(count(), recipeEdit, QString());
    setCurrentIndex(index);
    updateTabText(index);
    emit empty(false);
}


bool RecipeTabWidget::closeCurrentTab() {
    if (count() == 0) {
        return false;
    }

    return closeTab(currentIndex());
}


void RecipeTabWidget::closeAllTabs() {
    if (count() == 0)
        return;

    setCurrentIndex(0);
    while (count() > 0) {
        if (closeCurrentTab() == false)
            break;
    }
}


bool RecipeTabWidget::closeTab(int index) {
    RecipeEdit* edit = m_recipes[index];

    if (edit->hasUnsavedChanges()) {
        QString title = trUtf8("Close recipe");
        QString text = trUtf8("Save changes?");
        QMessageBox question(this);
        question.setWindowTitle(title);
        question.setText(text);
        question.setIcon(QMessageBox::Question);
        question.addButton(QMessageBox::Yes);
        question.addButton(QMessageBox::No);
        question.addButton(QMessageBox::Cancel);
        question.show();

        int ret = question.exec();
        if (ret == QMessageBox::Cancel)
            return false;
        else if (ret == QMessageBox::Yes)
            if (edit->save() == false)
                return false;
    }

    removeTab(index);
    m_recipes.removeAll(edit);
    delete edit;
    edit = 0;

    if (count() == 0)
        emit empty(true);

    return true;
}


RecipeEdit* RecipeTabWidget::currentRecipe() {
    if (count() == 0)
        return 0;
    else
        return m_recipes[currentIndex()];
}


void RecipeTabWidget::keyPressEvent(QKeyEvent* event) {
    int cnt = count();
    if (cnt == 0) {
        event->ignore();
        return;
    }        

    if (event->modifiers() == Qt::ControlModifier) {
        if (event->key() == Qt::Key_PageUp) {
            int nextIndex = (currentIndex() + 1) % cnt;
            setCurrentIndex(nextIndex);
        } else if (event->key() == Qt::Key_PageDown) {
            int nextIndex = (cnt + currentIndex() - 1) % cnt;
            setCurrentIndex(nextIndex);
        }
    }

    event->accept();
}


void RecipeTabWidget::newRecipe() {
    RecipeEdit* recipeEdit = new RecipeEdit(this);
    addRecipe(recipeEdit);
}


void RecipeTabWidget::openRecipe(QString fileName) {    
    if (fileName.isEmpty()) {
        QString caption = trUtf8("Open recipe");
        QString dir = QDir::homePath();
        QString filter = trUtf8("Recipe files (*.xml)");

        fileName = QFileDialog::getOpenFileName(this, caption, dir, filter);
        if (fileName.isEmpty() == true)
            return;
    }    

    // select tab if chosen file is already opened
    QList<RecipeEdit*>::Iterator it = m_recipes.begin();
    for (; it != m_recipes.end(); ++it) {
        if ((*it)->fileName() == fileName) {
            setCurrentWidget(*it);
            return;
        }
    }    

    // create new tab with the selected recipe
    RecipeEdit* recipeEdit = new RecipeEdit(this);    
    if (recipeEdit->fill(fileName) == true) {
        addRecipe(recipeEdit);
    } else {
        QMessageBox::critical(this, trUtf8("Open failed"), trUtf8("Error while loading recipe!"));
        delete recipeEdit;
        recipeEdit = 0;
    }
}


void RecipeTabWidget::recipeChanged(RecipeEdit *recipeEdit) {
    int index = indexOf(recipeEdit);
    if (index != -1) {
        setTabText(index, recipeEdit->fileName(false) + " (*)");
    }
}


void RecipeTabWidget::saveAllTabs() {
    if (count() == 0)
        return;

    for (int index = 0; index < count(); ++index) {
        setCurrentIndex(index);
        if (m_recipes[index]->save() == false)
            break;
        else
            updateTabText(index);
    }
}


void RecipeTabWidget::updateTabText(int index) {
    if (index < 0 || index >= count())
        return;

    RecipeEdit* edit = m_recipes[index];
    QString text = edit->fileName(false);
    if (edit->hasUnsavedChanges() == true)
        text += " (*)";

    setTabText(index, text);
}


void RecipeTabWidget::updateTabText(RecipeEdit* recipeEdit) {
    if (recipeEdit == 0)
        return;
    else
        updateTabText(indexOf(recipeEdit));
}
