#ifndef PREPARATIONSTEPDIALOG_HH
#define PREPARATIONSTEPDIALOG_HH


#include <QDialog>
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QPushButton>


class PreparationStepDialog : public QDialog {

    Q_OBJECT


public:

    PreparationStepDialog(QWidget* parent = 0);

    void setParagraphAllowed(bool allowed);
    void setText(QString text);
    QString text();


private:

    bool m_paragraphAllowed;
    QGridLayout* m_layout;
    QPlainTextEdit* m_edit;
    QPushButton* m_okay;
    QPushButton* m_cancel;

};

#endif // PREPARATIONSTEPDIALOG_HH
