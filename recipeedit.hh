#ifndef RECIPEEDIT_HH
#define RECIPEEDIT_HH


#include <QFont>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include "ingredientlistedit.hh"
#include "preparationlistedit.hh"


class RecipeEdit : public QWidget {

    Q_OBJECT


signals:

    void changed(RecipeEdit*);


public:

    RecipeEdit(QWidget* parent = 0);

    QString filename(bool withPath=true);
    bool fromXml(QString filename);
    bool hasUnsavedChanges();
    bool save();


public slots:

    void addIngredient();
    void addPreparationStep();
    void addSection();
    void editHeadline();
    void editServingCount();    


private slots:

    void triggerChanged();


private:

    void setupFonts();
    QString toXml();


    bool m_unsavedChanges;
    IngredientListEdit* m_ingredients;
    PreparationListEdit* m_preparation;
    QFont m_h1Font;
    QFont m_h2Font;
    QLabel* m_headline;
    QString m_filename;
    QVBoxLayout* m_layout;


};

#endif // RECIPEEDIT_HH
