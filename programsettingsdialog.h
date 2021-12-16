#ifndef PROGRAMSETTINGSDIALOG_H
#define PROGRAMSETTINGSDIALOG_H

#include <QSlider>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>

class ProgramSettingsDialog : public QDialog
{
public:
    ProgramSettingsDialog(int currentMaxSmokeTrackingDelaySeconds, QWidget* parent = nullptr);

    int getMaxSmokeTrackingDelaySeconds() const;

private:
    QSlider* m_maxSmokeSlider {nullptr};

    QPushButton* m_acceptButton {nullptr};
    QPushButton* m_rejectButton {nullptr};

    QVBoxLayout* m_mainLayout {nullptr};

    void createDialogButtonsLayout();
};

#endif // PROGRAMSETTINGSDIALOG_H
