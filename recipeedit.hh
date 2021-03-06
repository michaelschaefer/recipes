#ifndef RECIPEEDIT_HH
#define RECIPEEDIT_HH


#include <QFont>
#include <QLabel>
#include <QScrollArea>
#include <QSplitter>
#include <QTextEdit>
#include <QVBoxLayout>
#include "exporter.hh"
#include "ingredientlistedit.hh"
#include "library.hh"
#include "preparationlistedit.hh"
#include "recipedata.hh"


class RecipeEdit : public QSplitter {

    Q_OBJECT


signals:

    void changed(RecipeEdit*);
    void saved(RecipeEdit*);


public:

    RecipeEdit(QWidget* parent = 0);

    QString fileName(bool withPath = true);
    QString pathName();
    bool fill(QString fileName);
    bool hasUnsavedChanges();    
    RecipeData& recipeData();


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
    void updateLibrary();
    void updatePreview();


    bool m_unsavedChanges;
    Exporter* m_exporter;
    IngredientListEdit* m_ingredients;
    Library* m_library;
    PreparationListEdit* m_preparation;
    QFont m_h1Font;
    QFont m_h2Font;
    QLabel* m_headline;
    QScrollArea* m_editArea;
    QString m_fileName;
    QTextEdit* m_previewWidget;
    QVBoxLayout* m_layout;
    QWidget* m_editWidget;
    RecipeData m_recipeData;


};

#endif // RECIPEEDIT_HH
