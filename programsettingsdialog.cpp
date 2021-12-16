#include "programsettingsdialog.h"

#include <QGridLayout>
#include <QLabel>

ProgramSettingsDialog::ProgramSettingsDialog(int currentMaxSmokeTrackingDelaySeconds, QWidget* parent) : QDialog(parent)
{
    m_mainLayout = new QVBoxLayout(this);

    auto layout = new QGridLayout;

    m_maxSmokeSlider = new QSlider(this);
    m_maxSmokeSlider->setRange(10, 120);
    m_maxSmokeSlider->setValue(currentMaxSmokeTrackingDelaySeconds);
    m_maxSmokeSlider->setOrientation(Qt::Orientation::Horizontal);

    auto textLabel = new QLabel(QStringLiteral("Задержка определения макс. дыма (с)"), this);
    auto valueLabel = new QLabel(this);

    connect(m_maxSmokeSlider, &QSlider::valueChanged, valueLabel, [this, valueLabel] {
        valueLabel->setText(QStringLiteral("%1 сек.").arg(m_maxSmokeSlider->value()));
    });

    valueLabel->setText(QStringLiteral("%1 сек.").arg(m_maxSmokeSlider->value()));

    layout->addWidget(textLabel, 0, 0);
    layout->addWidget(m_maxSmokeSlider, 0, 1);
    layout->addWidget(valueLabel, 0, 2);

    m_mainLayout->addLayout(layout);

    createDialogButtonsLayout();
}

int ProgramSettingsDialog::getMaxSmokeTrackingDelaySeconds() const
{
    return m_maxSmokeSlider->value();
}

void ProgramSettingsDialog::createDialogButtonsLayout()
{
    m_acceptButton = new QPushButton("Применить");
    connect(m_acceptButton, &QPushButton::clicked, this, &ProgramSettingsDialog::accept);

    m_rejectButton = new QPushButton("Закрыть");
    connect(m_rejectButton, &QPushButton::clicked, this, &ProgramSettingsDialog::reject);

    QHBoxLayout* dialogButtonsLayout = new QHBoxLayout;
    dialogButtonsLayout->addWidget(m_acceptButton);
    dialogButtonsLayout->addWidget(m_rejectButton);
    dialogButtonsLayout->setAlignment(Qt::AlignRight);

    m_mainLayout->addLayout(dialogButtonsLayout);
}
