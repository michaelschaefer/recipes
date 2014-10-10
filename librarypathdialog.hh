#ifndef LIBRARYPATHDIALOG_HH
#define LIBRARYPATHDIALOG_HH


#include <QDialog>
#include <QGridLayout>
#include <QListWidget>
#include <QKeyEvent>
#include <QPushButton>


class LibraryPathDialog : public QDialog {

    Q_OBJECT


public:

    LibraryPathDialog(QWidget* parent = 0);
    QStringList getPathList();
    void setPathList(const QStringList &pathList);


private slots:

    void addPath();
    void keyReleaseEvent(QKeyEvent* event);
    void removeSelectedItems();


private:

    QGridLayout* m_gridLayout;
    QListWidget* m_listWidget;
    QPushButton* m_buttonAdd;
    QPushButton* m_buttonApply;
    QPushButton* m_buttonCancel;
    QPushButton* m_buttonRemove;

};

#endif // LIBRARYPATHDIALOG_HH
