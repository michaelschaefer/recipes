#ifndef SETTINGSTABLIBRARY_HH
#define SETTINGSTABLIBRARY_HH


#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QWidget>


class SettingsTabLibrary : public QWidget {

    Q_OBJECT


public:

    SettingsTabLibrary(QWidget* parent = 0);


private slots:

    void toggleLocation(int index);


private:

    QComboBox* m_comboBoxLocation;
    QFormLayout* m_layout;
    QGroupBox* m_groupBoxLocal;
    QGroupBox* m_groupBoxRemote;

};

#endif // SETTINGSTABLIBRARY_HH
