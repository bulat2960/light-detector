#ifndef STARTEXPERIMENTDIALOG_H
#define STARTEXPERIMENTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>

#include "protocolparameters.h"

class StartExperimentDialog : public QDialog
{
public:
    StartExperimentDialog(QWidget* parent = nullptr);

    ProtocolParameters getProtocolParameters() const;

private slots:
    void checkSettings();

private:
    ProtocolParameters m_protocolParameters;

    QVBoxLayout* m_layout {nullptr};

    QLineEdit* m_materialBrandEdit {nullptr};
    QLineEdit* m_batchNumberEdit {nullptr};
    QLineEdit* m_recipeEdit {nullptr};
    QLineEdit* m_weightAndThicknessEdit {nullptr};

    QLineEdit* m_testSeriesNumberEdit {nullptr};
    QLineEdit* m_densityEdit {nullptr};
    QComboBox* m_sampleTypeBox {nullptr};
    QLineEdit* m_firstNoteEdit {nullptr};
    QLineEdit* m_secondNoteEdit {nullptr};
    QComboBox* m_regimeBox {nullptr};

    void setupTemplateLayout();
    void setupParametersLayout();
    void setupDialogButtonsLayout();

};

#endif // STARTEXPERIMENTDIALOG_H
