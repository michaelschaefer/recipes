#ifndef RECIPEEDIT_HH
#define RECIPEEDIT_HH


#include <QFont>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include "ingredientlistedit.hh"
#include "preparationlistedit.hh"
#include "recipedata.hh"


class RecipeEdit : public QWidget {

    Q_OBJECT


signals:

    void changed(RecipeEdit*);
    void saved(RecipeEdit*);


public:

    RecipeEdit(QWidget* parent = 0);

    QString filename(bool withPath=true);
    bool fromXml(QString filename);
    bool hasUnsavedChanges();    
    RecipeData recipeData();    


public slots:

    void addIngredient();
    void addPreparationStep();
    void addSection();
    void editHeadline();
    void editServingCount();    
    void exportAsPdf();
    void print();
    bool save();
    bool saveAs();


private slots:

    void triggerChanged();


private:

    void setupFonts();


    bool m_unsavedChanges;
    IngredientListEdit* m_ingredients;
    PreparationListEdit* m_preparation;
    QFont m_h1Font;
    QFont m_h2Font;
    QLabel* m_headline;
    QScrollArea* m_editArea;
    QString m_filename;
    QVBoxLayout* m_layout;


};

#endif // RECIPEEDIT_HH
