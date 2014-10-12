#ifndef RECIPEEDIT_HH
#define RECIPEEDIT_HH


#include <QFont>
#include <QLabel>
#include <QScrollArea>
#include <QSplitter>
#include <QTextEdit>
#include <QVBoxLayout>
#include "ingredientlistedit.hh"
#include "preparationlistedit.hh"
#include "recipedata.hh"


class RecipeEdit : public QSplitter {

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
    void togglePreview(bool visible);


private slots:

    void triggerChanged();


private:

    void setupFonts();
    void updatePreview();


    bool m_unsavedChanges;
    IngredientListEdit* m_ingredients;
    PreparationListEdit* m_preparation;
    QFont m_h1Font;
    QFont m_h2Font;
    QLabel* m_headline;
    QScrollArea* m_editArea;
    QString m_filename;
    QTextEdit* m_previewWidget;
    QVBoxLayout* m_layout;
    QWidget* m_editWidget;


};

#endif // RECIPEEDIT_HH
