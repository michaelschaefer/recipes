#ifndef SYNCHRONIZATIONDIALOG_HH
#define SYNCHRONIZATIONDIALOG_HH


#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTreeView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QLabel>


class SynchronizationDialog : public QDialog {

    Q_OBJECT


public:

    enum Location {
        Local, Remote
    };


    SynchronizationDialog(QWidget* parent = 0);

    void setData(QList<QStringList> syncDataList);
    QStringList downloadFileList();
    QStringList uploadFileList();


private slots:

    void downloadAll();
    void itemChanged(QStandardItem* item);
    void uploadAll();


private:

    void initView();
    void setupButtonBox();


    bool m_boolLocalDataSet;
    bool m_boolRemoteDataSet;
    QDialogButtonBox* m_buttonBox;
    QLabel* m_labelHint;
    QStandardItemModel* m_treeViewModel;
    QTreeView* m_treeView;
    QPushButton* m_buttonDownload;
    QPushButton* m_buttonUpload;
    QVBoxLayout* m_layout;
    QList<QStringList> m_syncDataList;

};

#endif // SYNCHRONIZATIONDIALOG_HH
