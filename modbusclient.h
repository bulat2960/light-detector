#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include <QModbusRtuSerialClient>
#include <QTimer>

class ModbusClient : public QModbusRtuSerialClient
{
    Q_OBJECT
public:
    ModbusClient(int requestsInSecond, QObject* parent = nullptr);

    void setServerAddress(int serverAddress);

    void parseDeviceData();
    void checkConnection();
    void getCalibrationValue();

    void start();
    bool isActive() const;
    void stop();

signals:
    void dataParsed(double value);
    void calibrationValueReceived(double value);
    void connectionEstablished();

public slots:
    void sendDeviceStateRequest();
    void sendDataRequest();
    void sendCalibrationRequest();

private:
    int m_serverAddress;

    QTimer* m_requestTimer {nullptr};

    QModbusDataUnit createDeviceStateRequest();
    QModbusDataUnit createDataRequest();

    float pairOfUintToFloat(quint16 first, quint16 second);
};

#endif // MODBUSCLIENT_H
