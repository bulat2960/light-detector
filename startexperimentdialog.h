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

    QLineEdit* m_nameEdit {nullptr};
    QLineEdit* m_brandEdit {nullptr};
    QLineEdit* m_providerEdit {nullptr};
    QLineEdit* m_thicknessEdit {nullptr};

    QLineEdit* m_protocolNumberEdit {nullptr};
    QLineEdit* m_densityEdit {nullptr};
    QLineEdit* m_fioEdit {nullptr};
    QLineEdit* m_samplesNumberEdit {nullptr};
    QLineEdit* m_conditioningEdit {nullptr};
    QComboBox* m_regimeBox {nullptr};

    void setupTemplateLayout();
    void setupParametersLayout();
    void setupDialogButtonsLayout();

};

#endif // STARTEXPERIMENTDIALOG_H
