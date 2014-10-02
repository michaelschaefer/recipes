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
    setCurrentIndex(addTab(recipeEdit, QString()));
    updateTabTexts();
}


bool RecipeTabWidget::closeCurrentTab() {
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
        QString title = trUtf8("Close Recipe");
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
    return true;
}


void RecipeTabWidget::newRecipe() {
    RecipeEdit* recipeEdit = new RecipeEdit(this);
    addRecipe(recipeEdit);
}


void RecipeTabWidget::openRecipe() {
    QString caption = trUtf8("Open Recipe");
    QString dir = QDir::homePath();
    QString filter = trUtf8("Recipe files (*.xml)");

    QString filename = QFileDialog::getOpenFileName(this, caption, dir, filter);
    if (filename.isEmpty() == false) {

        QList<RecipeEdit*>::Iterator it = m_recipes.begin();
        for (; it != m_recipes.end(); ++it) {
            if ((*it)->filename() == filename) {
                setCurrentWidget(*it);
                return;
            }
        }

        RecipeEdit* recipeEdit = new RecipeEdit(this);
        if (recipeEdit->fromXml(filename) == true) {
            addRecipe(recipeEdit);
        } else {
            QMessageBox::critical(this, trUtf8("Open Failed"), trUtf8("Error while loading recipe!"));
            delete recipeEdit;
            recipeEdit = 0;
        }
    }
}


void RecipeTabWidget::recipeChanged(RecipeEdit *recipeEdit) {
    int index = indexOf(recipeEdit);
    if (index != -1) {
        setTabText(index, recipeEdit->filename(false) + " (*)");
    }
}


void RecipeTabWidget::saveAllTabs() {
    if (count() == 0)
        return;

    for (int i = 0; i < count(); ++i) {
        setCurrentIndex(i);
        if (saveCurrentTab() == false)
            break;
    }
}


bool RecipeTabWidget::saveCurrentTab() {
    if (m_recipes[currentIndex()]->save() == false) {
        QMessageBox::critical(this, trUtf8("Save Failed"), trUtf8("Error while saving recipe!"));
        return false;
    } else {
        updateTabTexts();
        return true;
    }
}


void RecipeTabWidget::updateTabTexts() {
    QString text;
    RecipeEdit* edit = 0;

    for (int i = 0; i < count(); ++i) {
        edit = (RecipeEdit*)widget(i);
        text = edit->filename(false);
        if (edit->hasUnsavedChanges() == true) {
            text += " (*)";
        }
        setTabText(i, text);
    }
}
