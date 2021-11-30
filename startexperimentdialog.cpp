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

    QLabel* materialBrandLabel = new QLabel("Марка материала", this);
    m_materialBrandEdit = new QLineEdit(this);

    QLabel* batchNumberLabel = new QLabel("Номер партии", this);
    m_batchNumberEdit = new QLineEdit(this);

    QLabel* recipeLabel = new QLabel("Рецептура", this);
    m_recipeEdit = new QLineEdit(this);

    QLabel* weightAndThicknessLabel = new QLabel("Масса (г), толщина (мм)", this);
    m_weightAndThicknessEdit = new QLineEdit(this);

    templateLayout->addWidget(materialBrandLabel, 0, 0);
    templateLayout->addWidget(m_materialBrandEdit, 1, 0);

    templateLayout->addWidget(batchNumberLabel, 0, 1);
    templateLayout->addWidget(m_batchNumberEdit, 1, 1);

    templateLayout->addWidget(recipeLabel, 2, 0);
    templateLayout->addWidget(m_recipeEdit, 3, 0);

    templateLayout->addWidget(weightAndThicknessLabel, 2, 1);
    templateLayout->addWidget(m_weightAndThicknessEdit, 3, 1);

    m_layout->addWidget(templateLabel);
    m_layout->addLayout(templateLayout);
}

void StartExperimentDialog::setupParametersLayout()
{
    QLabel* parametersLabel = new QLabel("Параметры", this);
    parametersLabel->setStyleSheet(QStringLiteral("QLabel { background-color: #add8e6; font-size: 20px };"));

    QGridLayout* parametersLayout = new QGridLayout;

    QLabel* testSeriesNumber = new QLabel("Номер в серии испытаний", this);
    m_testSeriesNumberEdit = new QLineEdit(this);
    m_testSeriesNumberEdit->setText("0");

    QLabel* densityLabel = new QLabel("Плотность теплового потока (Вт/м2)", this);
    m_densityEdit = new QLineEdit(this);
    m_densityEdit->setText("0");

    QLabel* sampleTypeLabel = new QLabel("Тип образца", this);
    m_sampleTypeBox = new QComboBox(this);
    m_sampleTypeBox->addItems({"Цельный", "Сборный"});

    QLabel* firstNoteLabel = new QLabel("Примечания 1", this);
    m_firstNoteEdit = new QLineEdit(this);

    QLabel* secondNoteLabel = new QLabel("Примечания 2", this);
    m_secondNoteEdit = new QLineEdit(this);

    QLabel* regimeLabel = new QLabel("Режим", this);
    m_regimeBox = new QComboBox(this);
    m_regimeBox->addItems({"Пламенное горение", "Тление"});

    parametersLayout->addWidget(testSeriesNumber, 0, 0);
    parametersLayout->addWidget(m_testSeriesNumberEdit, 1, 0);

    parametersLayout->addWidget(densityLabel, 0, 1);
    parametersLayout->addWidget(m_densityEdit, 1, 1);

    parametersLayout->addWidget(sampleTypeLabel, 2, 0);
    parametersLayout->addWidget(m_sampleTypeBox, 3, 0);

    parametersLayout->addWidget(regimeLabel, 2, 1);
    parametersLayout->addWidget(m_regimeBox, 3, 1);

    parametersLayout->addWidget(firstNoteLabel, 4, 0);
    parametersLayout->addWidget(m_firstNoteEdit, 5, 0);

    parametersLayout->addWidget(secondNoteLabel, 4, 1);
    parametersLayout->addWidget(m_secondNoteEdit, 5, 1);

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
    m_protocolParameters.materialBrand = m_materialBrandEdit->text();
    m_protocolParameters.batchNumber = m_batchNumberEdit->text();
    m_protocolParameters.recipe = m_recipeEdit->text();
    m_protocolParameters.weightAndThickness = m_weightAndThicknessEdit->text();

    m_protocolParameters.testSeriesNumber = m_testSeriesNumberEdit->text();
    m_protocolParameters.density = m_densityEdit->text();
    m_protocolParameters.sampleType = m_sampleTypeBox->currentText();
    m_protocolParameters.firstNote = m_firstNoteEdit->text();
    m_protocolParameters.secondNote = m_secondNoteEdit->text();
    m_protocolParameters.regime = m_regimeBox->currentText();

    QDialog::accept();
}

ProtocolParameters StartExperimentDialog::getProtocolParameters() const
{
    return m_protocolParameters;
}
