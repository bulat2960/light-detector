#include "modbusclient.h"

#include <QDebug>
#include <QBuffer>
#include <QDateTime>

#include "logger.h"

ModbusClient::ModbusClient(int requestsInSecond, QObject* parent)
    : QModbusRtuSerialClient(parent)
{
    assert(requestsInSecond != 0);

    m_requestTimer = new QTimer(this);
    m_requestTimer->setSingleShot(false);
    m_requestTimer->setInterval(1000 / requestsInSecond);
    connect(m_requestTimer, &QTimer::timeout, this, &ModbusClient::sendDataRequest);
}

void ModbusClient::start()
{
    m_requestTimer->start();
}

void ModbusClient::stop()
{
    m_requestTimer->stop();
}

bool ModbusClient::isActive() const
{
    return m_requestTimer->isActive();
}

void ModbusClient::setServerAddress(int serverAddress)
{
    m_serverAddress = serverAddress;
}

void ModbusClient::sendDataRequest()
{
    QModbusDataUnit unit = createDataRequest();
    QModbusReply* reply = sendReadRequest(unit, m_serverAddress);

    if (not reply)
    {
        Logger::instance().logMessage(Logger::Type::MODBUS,
                                      QStringLiteral("Send data request: null reply"));
        return;
    }

    connect(reply, &QModbusReply::finished, this, &ModbusClient::parseDeviceData);
}

void ModbusClient::sendDeviceStateRequest()
{
    QModbusDataUnit unit = createDeviceStateRequest();
    QModbusReply* reply = sendReadRequest(unit, m_serverAddress);

    if (not reply)
    {
        Logger::instance().logMessage(Logger::Type::MODBUS,
                                      QStringLiteral("Send device state request: null reply"));
        return;
    }

    connect(reply, &QModbusReply::finished, this, &ModbusClient::checkConnection);

    Logger::instance().logMessage(Logger::Type::MODBUS,
                                  QStringLiteral("Device state request sent"));
}

void ModbusClient::sendCalibrationRequest()
{
    QModbusDataUnit unit = createDataRequest();
    QModbusReply* reply = sendReadRequest(unit, m_serverAddress);

    if (not reply)
    {
        Logger::instance().logMessage(Logger::Type::MODBUS,
                                      QStringLiteral("Send calibration request: null reply"));
        return;
    }

    connect(reply, &QModbusReply::finished, this, &ModbusClient::getCalibrationValue);

    Logger::instance().logMessage(Logger::Type::MODBUS,
                                  QStringLiteral("Calibration value request sent"));
}

QModbusDataUnit ModbusClient::createDataRequest()
{
    quint16 startAddress = 0;
    quint16 size = 2;

    return QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, size);
}

QModbusDataUnit ModbusClient::createDeviceStateRequest()
{
    quint16 startAddress = 5;
    quint16 size = 1;

    return QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, size);
}

void ModbusClient::parseDeviceData()
{
    auto reply = qobject_cast<QModbusReply*>(sender());

    if (not reply)
    {
        return;
    }

    float value = std::numeric_limits<double>::quiet_NaN();

    if (reply->error() == QModbusDevice::NoError)
    {
        const QModbusDataUnit unit = reply->result();

        quint16 firstValue = unit.value(0);
        quint16 secondValue = unit.value(1);

        value = pairOfUintToFloat(firstValue, secondValue);
    }
    else
    {
        Logger::instance().logMessage(Logger::Type::MODBUS,
                                      QStringLiteral("Device data parsing failure: %1 (code: 0x%2)")
                                      .arg(reply->errorString())
                                      .arg(reply->error(), -1, 16));
    }

    reply->deleteLater();

    emit dataParsed(value);

    Logger::instance().logMessage(Logger::Type::MODBUS,
                                  QStringLiteral("Device data: %1").arg(value));
}

void ModbusClient::checkConnection()
{
    auto reply = qobject_cast<QModbusReply*>(sender());

    if (not reply)
    {
        return;
    }

    if (reply->error() == QModbusDevice::NoError)
    {
        Logger::instance().logMessage(Logger::Type::MODBUS,
                                      QStringLiteral("Connection established"));

        emit connectionEstablished();
    }
    else
    {
        Logger::instance().logMessage(Logger::Type::MODBUS,
                                      QStringLiteral("Connection check failure: %1 (code: 0x%2)")
                                      .arg(reply->errorString())
                                      .arg(reply->error(), -1, 16));
    }

    reply->deleteLater();
}

void ModbusClient::getCalibrationValue()
{
    auto reply = qobject_cast<QModbusReply*>(sender());

    if (not reply)
    {
        return;
    }

    float value = 0;

    if (reply->error() == QModbusDevice::NoError)
    {
        const QModbusDataUnit unit = reply->result();

        quint16 firstValue = unit.value(0);
        quint16 secondValue = unit.value(1);

        value = pairOfUintToFloat(firstValue, secondValue);

        Logger::instance().logMessage(Logger::Type::MODBUS,
                                      QStringLiteral("Calibration value set to %1").arg(value));

        emit calibrationValueReceived(value);
    }
    else
    {
        Logger::instance().logMessage(Logger::Type::MODBUS,
                                      QStringLiteral("Calibration value parsing failure: %1 (code: 0x%2)")
                                      .arg(reply->errorString())
                                      .arg(reply->error(), -1, 16));

        emit calibrationValueReceived(-1);
    }

    reply->deleteLater();

}

float ModbusClient::pairOfUintToFloat(quint16 first, quint16 second)
{
    float value = 0;

    QDataStream stream;
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    stream.setDevice(&buffer);

    stream << first << second;
    buffer.seek(0);
    stream >> value;

    return value;
}
