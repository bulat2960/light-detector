#include "connectionsettingsdialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>
#include <QSerialPort>

ConnectionSettingsDialog::ConnectionSettingsDialog(ModbusClient* client, QWidget *parent)
    : QDialog(parent),
      m_client(client)
{
    setWindowTitle(QStringLiteral("Настройки подключения"));
    setMinimumWidth(300);

    m_mainLayout = new QVBoxLayout(this);

    createDataElementsGrid();
    createPortElementsGrid();
    createDialogButtonsLayout();

    QTimer::singleShot(100, this, &ConnectionSettingsDialog::scanExistingPorts);

    if (m_client->state() == QModbusDevice::ConnectedState)
    {
        m_client->disconnectDevice();
    }
}

void ConnectionSettingsDialog::createDataElementsGrid()
{
    m_portBox = new QComboBox(this);
    connect(m_portBox, &QComboBox::currentIndexChanged, this, &ConnectionSettingsDialog::changeBaudRates);

    m_baudRateBox = new QComboBox(this);

    m_dataBitsBox = new QComboBox(this);
    m_dataBitsBox->addItems({"5", "6", "7", "8"});
    m_dataBitsBox->setCurrentText("8");

    m_parityBox = new QComboBox(this);
    m_parityBox->addItems({"None", "Even", "Odd", "Space", "Mark"});

    m_stopBitsBox = new QComboBox(this);
    m_stopBitsBox->addItems({"One", "One and half", "Two"});

    m_timeoutBox = new QComboBox(this);
    m_timeoutBox->addItems({"100", "200", "300"});
    m_timeoutBox->setCurrentText("200");

    m_addressLineEdit = new QLineEdit(this);
    m_addressLineEdit->setText("1");


    QGridLayout* dataLayout = new QGridLayout;

    dataLayout->addWidget(new QLabel("Порт, COM"), 0, 0);
    dataLayout->addWidget(m_portBox, 0, 1);

    dataLayout->addWidget(new QLabel("Скорость, бит/с"), 1, 0);
    dataLayout->addWidget(m_baudRateBox, 1, 1);

    dataLayout->addWidget(new QLabel("Число бит данных, бит"), 2, 0);
    dataLayout->addWidget(m_dataBitsBox, 2, 1);

    dataLayout->addWidget(new QLabel("Контроль четности"), 3, 0);
    dataLayout->addWidget(m_parityBox, 3, 1);

    dataLayout->addWidget(new QLabel("Число стоповых бит, бит"), 4, 0);
    dataLayout->addWidget(m_stopBitsBox, 4, 1);

    dataLayout->addWidget(new QLabel("Ожидание чтения, мс"), 5, 0);
    dataLayout->addWidget(m_timeoutBox, 5, 1);

    dataLayout->addWidget(new QLabel("Адрес"), 6, 0);
    dataLayout->addWidget(m_addressLineEdit, 6, 1);

    m_mainLayout->addLayout(dataLayout);
}

void ConnectionSettingsDialog::createPortElementsGrid()
{
    m_checkConnectionButton = new QPushButton("Проверка связи", this);
    m_checkConnectionButton->setDisabled(true);
    connect(m_checkConnectionButton, &QPushButton::clicked, m_client, &ModbusClient::sendDeviceStateRequest);
    connect(m_client, &ModbusClient::connectionEstablished, this, &ConnectionSettingsDialog::onConnectionEstablished);

    m_portNameLabel = new QLabel(this);
    m_portNameLabel->setText(QStringLiteral("Порт закрыт"));
    m_portNameLabel->setStyleSheet(QStringLiteral("QLabel { background-color: lightgray };"));

    m_connectionEstablishedLabel = new QLabel(this);
    m_connectionEstablishedLabel->setText(QStringLiteral("Соединение активно"));
    m_connectionEstablishedLabel->setStyleSheet(QStringLiteral("QLabel { background-color: lightgreen };"));
    m_connectionEstablishedLabel->hide();

    m_openPortButton = new QPushButton("Открыть порт", this);

    connect(m_openPortButton, &QPushButton::clicked, this, [this] {
        openPort();

        m_openPortButton->setEnabled(false);
        m_checkConnectionButton->setEnabled(true);
        m_portBox->setEnabled(false);
        m_baudRateBox->setEnabled(false);
        m_dataBitsBox->setEnabled(false);
        m_parityBox->setEnabled(false);
        m_stopBitsBox->setEnabled(false);
        m_timeoutBox->setEnabled(false);
        m_addressLineEdit->setEnabled(false);

        m_portNameLabel->setText(m_portBox->currentText());
        m_portNameLabel->setStyleSheet(QStringLiteral("QLabel { background-color: lightgreen };"));
    });

    QGridLayout* portLayout = new QGridLayout;

    portLayout->addWidget(m_openPortButton, 0, 0);
    portLayout->addWidget(m_portNameLabel, 0, 1);
    portLayout->addWidget(m_checkConnectionButton, 1, 0);
    portLayout->addWidget(m_connectionEstablishedLabel, 1, 1);

    m_mainLayout->addLayout(portLayout);
}

void ConnectionSettingsDialog::createDialogButtonsLayout()
{
    m_acceptButton = new QPushButton("Применить");
    m_acceptButton->setEnabled(false);
    connect(m_acceptButton, &QPushButton::clicked, this, &ConnectionSettingsDialog::accept);

    m_rejectButton = new QPushButton("Закрыть");
    connect(m_rejectButton, &QPushButton::clicked, this, &ConnectionSettingsDialog::reject);

    QHBoxLayout* dialogButtonsLayout = new QHBoxLayout;
    dialogButtonsLayout->addWidget(m_acceptButton);
    dialogButtonsLayout->addWidget(m_rejectButton);
    dialogButtonsLayout->setAlignment(Qt::AlignRight);

    m_mainLayout->addLayout(dialogButtonsLayout);
}

void ConnectionSettingsDialog::openPort()
{
    QMap<QString, int> parityMap =
    {
        {QStringLiteral("None"),  QSerialPort::NoParity},
        {QStringLiteral("Even"),  QSerialPort::EvenParity},
        {QStringLiteral("Odd"),   QSerialPort::OddParity},
        {QStringLiteral("Space"), QSerialPort::SpaceParity},
        {QStringLiteral("Mark"),  QSerialPort::MarkParity}
    };


    QMap<QString, int> stopBitsMap =
    {
        {QStringLiteral("One"),          QSerialPort::OneStop},
        {QStringLiteral("One and half"), QSerialPort::OneAndHalfStop},
        {QStringLiteral("Two"),          QSerialPort::TwoStop}
    };

    QString name = m_portBox->currentText();
    int parity = parityMap[m_parityBox->currentText()];
    int baudRate = m_baudRateBox->currentText().toInt();
    int dataBits = m_dataBitsBox->currentText().toInt();
    int stopBits = stopBitsMap[m_stopBitsBox->currentText()];
    int timeout = m_timeoutBox->currentText().toInt();
    int address = m_addressLineEdit->text().toInt();

    m_client->setConnectionParameter(QModbusDevice::SerialPortNameParameter, name);
    m_client->setConnectionParameter(QModbusDevice::SerialParityParameter,   parity);
    m_client->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, baudRate);
    m_client->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, dataBits);
    m_client->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, stopBits);
    m_client->setServerAddress(address);

    m_client->setTimeout(timeout);
    m_client->setNumberOfRetries(0);

    if (not m_client->connectDevice())
    {
        QMessageBox box;
        box.setText(QStringLiteral("Не удалось установить соединение. Проверьте, подключено ли устройство."));
        box.exec();

        QDialog::reject();
    }

    m_checkConnectionButton->setDisabled(true);
    m_acceptButton->setEnabled(true);
}

void ConnectionSettingsDialog::scanExistingPorts()
{
    QSerialPortInfo info;
    QList<QSerialPortInfo> availablePorts = info.availablePorts();

    if (availablePorts.empty())
    {
        QMessageBox box;
        box.setText(QStringLiteral("Не найдены доступные COM-порты."));
        box.exec();

        QDialog::reject();
    }

    for (const QSerialPortInfo& info : availablePorts)
    {
        m_portBox->addItem(info.portName());
        portBaudRates.insert(info.portName(), info.standardBaudRates());
    }

    changeBaudRates();
}

void ConnectionSettingsDialog::onConnectionEstablished()
{
    m_checkConnectionButton->setEnabled(false);
    m_connectionEstablishedLabel->show();
}

void ConnectionSettingsDialog::changeBaudRates()
{
    QList<int> baudRatesList = portBaudRates[m_portBox->currentText()];

    for (int value : baudRatesList)
    {
        m_baudRateBox->addItem(QString::number(value));
    }

    m_baudRateBox->setCurrentText("9600");
}
