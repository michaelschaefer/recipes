#ifndef PREPARATIONLISTEDIT_HH
#define PREPARATIONLISTEDIT_HH


#include "flexiblelayout.hh"


class PreparationListEdit : public QWidget {

    Q_OBJECT


signals:

    void changed();


public:

    PreparationListEdit(QWidget* parent = 0);

    void addPreparationStep();
    void addPreparationStep(QString text);
    QList<QMap<QString, QString> > data();
    void updateHeadline();



private slots:

    void triggerChanged();


private:

    FlexibleLayout* m_flexibleLayout;
    QLabel* m_headline;
    QVBoxLayout* m_layout;

};

#endif // PREPARATIONLISTEDIT_HH
