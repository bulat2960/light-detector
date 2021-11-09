#include "startexperimentdialog.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>

StartExperimentDialog::StartExperimentDialog(QWidget* parent)
    : QDialog(parent)
{
    m_layout = new QVBoxLayout(this);

    setMinimumWidth(500);

    setupTemplateLayout();
    setupParametersLayout();
    setupDialogButtonsLayout();
}

void StartExperimentDialog::setupTemplateLayout()
{
    QLabel* templateLabel = new QLabel("Образец", this);
    templateLabel->setStyleSheet(QStringLiteral("QLabel { background-color: #add8e6; font-size: 20px };"));

    QGridLayout* templateLayout = new QGridLayout;

    QLabel* nameLabel = new QLabel("Наименование", this);
    m_nameEdit = new QLineEdit(this);

    QLabel* brandLabel = new QLabel("Марка", this);
    m_brandEdit = new QLineEdit(this);

    QLabel* providerLabel = new QLabel("Поставщик", this);
    m_providerEdit = new QLineEdit(this);

    QLabel* thicknessLabel = new QLabel("Толщина, мм", this);
    m_thicknessEdit = new QLineEdit(this);
    m_thicknessEdit->setText("0");

    templateLayout->addWidget(nameLabel, 0, 0);
    templateLayout->addWidget(m_nameEdit, 1, 0);

    templateLayout->addWidget(brandLabel, 0, 1);
    templateLayout->addWidget(m_brandEdit, 1, 1);

    templateLayout->addWidget(providerLabel, 2, 0);
    templateLayout->addWidget(m_providerEdit, 3, 0);

    templateLayout->addWidget(thicknessLabel, 2, 1);
    templateLayout->addWidget(m_thicknessEdit, 3, 1);

    m_layout->addWidget(templateLabel);
    m_layout->addLayout(templateLayout);
}

void StartExperimentDialog::setupParametersLayout()
{
    QLabel* parametersLabel = new QLabel("Параметры", this);
    parametersLabel->setStyleSheet(QStringLiteral("QLabel { background-color: #add8e6; font-size: 20px };"));

    QGridLayout* parametersLayout = new QGridLayout;

    QLabel* protocolNumberLabel = new QLabel("Номер протокола", this);
    m_protocolNumberEdit = new QLineEdit(this);
    m_protocolNumberEdit->setText("0");

    QLabel* densityLabel = new QLabel("Плотность теплового потока, Вт/м2", this);
    m_densityEdit = new QLineEdit(this);
    m_densityEdit->setText("0");

    QLabel* fioLabel = new QLabel("Фамилия испытателя", this);
    m_fioEdit = new QLineEdit(this);

    QLabel* samplesNumberLabel = new QLabel("Число образцов", this);
    m_samplesNumberEdit = new QLineEdit(this);
    m_samplesNumberEdit->setText("0");

    QLabel* conditioningLabel = new QLabel("Кондиционирование", this);
    m_conditioningEdit = new QLineEdit(this);

    QLabel* regimeLabel = new QLabel("Режим", this);
    m_regimeBox = new QComboBox(this);
    m_regimeBox->addItems({"Пламенное горение", "Тление"});

    parametersLayout->addWidget(protocolNumberLabel, 0, 0);
    parametersLayout->addWidget(m_protocolNumberEdit, 1, 0);

    parametersLayout->addWidget(densityLabel, 0, 1);
    parametersLayout->addWidget(m_densityEdit, 1, 1);

    parametersLayout->addWidget(fioLabel, 2, 0);
    parametersLayout->addWidget(m_fioEdit, 3, 0);

    parametersLayout->addWidget(samplesNumberLabel, 2, 1);
    parametersLayout->addWidget(m_samplesNumberEdit, 3, 1);

    parametersLayout->addWidget(conditioningLabel, 4, 0);
    parametersLayout->addWidget(m_conditioningEdit, 5, 0);

    parametersLayout->addWidget(regimeLabel, 4, 1);
    parametersLayout->addWidget(m_regimeBox, 5, 1);

    m_layout->addWidget(parametersLabel);
    m_layout->addLayout(parametersLayout);
}

void StartExperimentDialog::setupDialogButtonsLayout()
{
    QPushButton* acceptButton = new QPushButton("Начать");
    connect(acceptButton, &QPushButton::clicked, this, &StartExperimentDialog::checkSettings);

    QPushButton* rejectButton = new QPushButton("Отмена");
    connect(rejectButton, &QPushButton::clicked, this, &StartExperimentDialog::reject);

    QHBoxLayout* dialogButtonsLayout = new QHBoxLayout;
    dialogButtonsLayout->addWidget(acceptButton);
    dialogButtonsLayout->addWidget(rejectButton);
    dialogButtonsLayout->setAlignment(Qt::AlignRight);

    m_layout->addLayout(dialogButtonsLayout);
}

void StartExperimentDialog::checkSettings()
{
    bool ok;
    QMessageBox box;
    box.setIcon(QMessageBox::Information);
    box.setWindowTitle(QStringLiteral("Внимание!"));

    QString name = m_nameEdit->text();
    QString brand = m_brandEdit->text();
    QString provider = m_providerEdit->text();

    double thickness = m_thicknessEdit->text().toDouble(&ok);
    if (not ok)
    {
        box.setText(QStringLiteral("Толщина установлена неверно. Введите корректное вещественное число."));
        box.exec();
        return;
    }

    int protocolNumber = m_protocolNumberEdit->text().toInt(&ok);
    if (not ok)
    {
        box.setText(QStringLiteral("Номер протокола установлен неверно. Введите корректное целое число."));
        box.exec();
        return;
    }

    double density = m_densityEdit->text().toDouble(&ok);
    if (not ok)
    {
        box.setText(QStringLiteral("Плотность потока установлена неверно. Введите корректное вещественное число."));
        box.exec();
        return;
    }

    QString fio = m_fioEdit->text();

    int samplesNumber = m_samplesNumberEdit->text().toInt(&ok);
    if (not ok)
    {
        box.setText(QStringLiteral("Число образцов установлено неверно. Введите корректное целое число."));
        box.exec();
        return;
    }

    QString conditioning = m_conditioningEdit->text();
    QString regime = m_regimeBox->currentText();


    m_protocolParameters.name = name;
    m_protocolParameters.brand = brand;
    m_protocolParameters.provider = provider;
    m_protocolParameters.thickness = thickness;

    m_protocolParameters.protocolNumber = protocolNumber;
    m_protocolParameters.density = density;
    m_protocolParameters.fio = fio;
    m_protocolParameters.samplesNumber = samplesNumber;
    m_protocolParameters.conditioning = conditioning;
    m_protocolParameters.regime = regime;

    QDialog::accept();
}

ProtocolParameters StartExperimentDialog::getProtocolParameters() const
{
    return m_protocolParameters;
}
