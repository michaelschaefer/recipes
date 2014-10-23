#ifndef SETTINGSTABFORMAT_HH
#define SETTINGSTABFORMAT_HH


#include <QFontComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QWidget>


class SettingsTabFormat : public QWidget {

    Q_OBJECT


public:

    struct FormatSettings {
        bool fontDefault;
        QString fontFamilyName;
        bool fontUser;
        bool paragraphDefault;
        QString paragraphEmptyHeadline;
        QString paragraphIngredients;
        QString paragraphPreparation;
        bool paragraphUser;
    };

    SettingsTabFormat(QWidget* parent = 0);

    void setSettings(FormatSettings formatSettings);
    FormatSettings settings();


private:

    void setupGroupBoxFont();
    void setupGroupBoxParagraphTitles();


    QFontComboBox* m_comboBoxFont;
    QFormLayout* m_layoutParagraphTitlesForm;
    QLineEdit* m_editParagraphTitleEmptyHeadline;
    QLineEdit* m_editParagraphTitleIngredients;
    QLineEdit* m_editParagraphTitlePreparation;
    QRadioButton* m_radioButtonDefaultFont;
    QRadioButton* m_radioButtonDefaultParagraph;
    QRadioButton* m_radioButtonUserFont;
    QRadioButton* m_radioButtonUserParagraph;
    QGroupBox* m_groupBoxFont;
    QGroupBox* m_groupBoxParagraphTitles;
    QVBoxLayout* m_layout;
    QVBoxLayout* m_layoutFormat;
    QVBoxLayout* m_layoutParagraphTitles;

};

#endif // SETTINGSTABFORMAT_HH
