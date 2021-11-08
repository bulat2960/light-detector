#include "settingsdialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>
#include <QSerialPort>

SettingsDialog::SettingsDialog(ModbusClient* client, QWidget *parent)
    : QDialog(parent),
      m_client(client)
{
    setWindowTitle(QStringLiteral("Настройки подключения"));
    setMinimumWidth(300);

    m_mainLayout = new QVBoxLayout(this);

    createDataElementsGrid();
    createPortElementsGrid();
    createDialogButtonsLayout();

    QTimer* scanTimer = new QTimer(this);
    connect(scanTimer, &QTimer::timeout, this, &SettingsDialog::scanExistingPorts);

    scanTimer->setSingleShot(true);
    scanTimer->setInterval(100);
    scanTimer->start();

    if (m_client->state() == QModbusDevice::ConnectedState)
    {
        m_client->disconnectDevice();
    }
}

void SettingsDialog::createDataElementsGrid()
{
    m_portBox = new QComboBox(this);
    connect(m_portBox, &QComboBox::currentIndexChanged, this, &SettingsDialog::changeBaudRates);

    m_baudRateBox = new QComboBox(this);

    m_dataBitsBox = new QComboBox(this);
    m_dataBitsBox->addItems({"5", "6", "7", "8"});
    m_dataBitsBox->setCurrentText("8");

    m_parityBox = new QComboBox(this);
    m_parityBox->addItems({"None", "Even", "Odd", "Space", "Mark"});

    m_stopBitsBox = new QComboBox(this);
    m_stopBitsBox->addItems({"One", "One and half", "Two"});

    m_readTimeoutBox = new QComboBox(this);
    m_readTimeoutBox->addItems({"100", "200", "300"});
    m_readTimeoutBox->setCurrentText("200");

    m_writeTimeoutBox = new QComboBox(this);
    m_writeTimeoutBox->addItems({"1000"});

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
    dataLayout->addWidget(m_readTimeoutBox, 5, 1);

    dataLayout->addWidget(new QLabel("Ожидание записи, мс"), 6, 0);
    dataLayout->addWidget(m_writeTimeoutBox, 6, 1);

    dataLayout->addWidget(new QLabel("Адрес"), 7, 0);
    dataLayout->addWidget(m_addressLineEdit, 7, 1);

    m_mainLayout->addLayout(dataLayout);
}

void SettingsDialog::createPortElementsGrid()
{
    m_checkConnectionButton = new QPushButton("Проверка связи", this);
    m_checkConnectionButton->setDisabled(true);
    connect(m_checkConnectionButton, &QPushButton::clicked, m_client, &ModbusClient::sendDeviceStateRequest);
    connect(m_client, &ModbusClient::connectionStateChecked, this, &SettingsDialog::parseDeviceState);

    m_portNameLabel = new QLabel(this);
    m_portNameLabel->setText(QStringLiteral("Порт закрыт"));
    m_portNameLabel->setStyleSheet(QStringLiteral("QLabel { background-color: lightgray };"));

    m_connectionEstablishedLabel = new QLabel(this);
    m_connectionEstablishedLabel->setText(QStringLiteral("SH v200 28-03-2017"));
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
        m_readTimeoutBox->setEnabled(false);
        m_writeTimeoutBox->setEnabled(false);
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

void SettingsDialog::createDialogButtonsLayout()
{
    m_acceptButton = new QPushButton("Применить");
    m_acceptButton->setEnabled(false);
    connect(m_acceptButton, &QPushButton::clicked, this, &SettingsDialog::accept);

    m_rejectButton = new QPushButton("Закрыть");
    connect(m_rejectButton, &QPushButton::clicked, this, &SettingsDialog::reject);

    QHBoxLayout* dialogButtonsLayout = new QHBoxLayout;
    dialogButtonsLayout->addWidget(m_acceptButton);
    dialogButtonsLayout->addWidget(m_rejectButton);
    dialogButtonsLayout->setAlignment(Qt::AlignRight);

    m_mainLayout->addLayout(dialogButtonsLayout);
}

void SettingsDialog::openPort()
{
    QMap<QString, int> parityMap;
    parityMap.insert(QStringLiteral("None"),  QSerialPort::NoParity);
    parityMap.insert(QStringLiteral("Even"),  QSerialPort::EvenParity);
    parityMap.insert(QStringLiteral("Odd"),   QSerialPort::OddParity);
    parityMap.insert(QStringLiteral("Space"), QSerialPort::SpaceParity);
    parityMap.insert(QStringLiteral("Mark"),  QSerialPort::MarkParity);

    int parity = parityMap[m_parityBox->currentText()];

    QMap<QString, int> stopBitsMap;
    stopBitsMap.insert(QStringLiteral("One"),          QSerialPort::OneStop);
    stopBitsMap.insert(QStringLiteral("One and half"), QSerialPort::OneAndHalfStop);
    stopBitsMap.insert(QStringLiteral("Two"),          QSerialPort::TwoStop);

    QString name = m_portBox->currentText();
    int baudRate = m_baudRateBox->currentText().toInt();
    int dataBits = m_dataBitsBox->currentText().toInt();
    int stopBits = stopBitsMap[m_stopBitsBox->currentText()];
    int readTimeout = m_readTimeoutBox->currentText().toInt();
    int writeTimeout = m_writeTimeoutBox->currentText().toInt(); (void)writeTimeout; // TODO: WTF is this parameter for?
    int address = m_addressLineEdit->text().toInt();

    m_client->setConnectionParameter(QModbusDevice::SerialPortNameParameter, name);
    m_client->setConnectionParameter(QModbusDevice::SerialParityParameter,   parity);
    m_client->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, baudRate);
    m_client->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, dataBits);
    m_client->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, stopBits);
    m_client->setServerAddress(address);

    m_client->setTimeout(readTimeout);
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

void SettingsDialog::scanExistingPorts()
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

void SettingsDialog::parseDeviceState(int value)
{
    // TODO: parse device state
    Q_UNUSED(value);
    m_checkConnectionButton->setEnabled(false);
    m_connectionEstablishedLabel->show();
}

void SettingsDialog::changeBaudRates()
{
    QList<int> baudRatesList = portBaudRates[m_portBox->currentText()];

    for (int value : baudRatesList)
    {
        m_baudRateBox->addItem(QString::number(value));
    }

    m_baudRateBox->setCurrentText("9600");
}
