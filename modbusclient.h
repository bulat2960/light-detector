#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include <QModbusRtuSerialClient>

class ModbusClient : public QModbusRtuSerialClient
{
    Q_OBJECT
public:
    ModbusClient(QObject* parent = nullptr);

    void setServerAddress(int serverAddress);

    void parseDeviceData();
    void checkConnection();
    void getCalibrationValue();

signals:
    void dataParsed(double value);
    void calibrationValueReceived(double value);
    void connectionStateChecked(int value);

public slots:
    void sendDeviceStateRequest();
    void sendDataRequest();
    void sendCalibrationRequest();

private:
    int m_serverAddress;

    QModbusDataUnit createDeviceStateRequest();
    QModbusDataUnit createDataRequest();

    float pairOfUintToFloat(quint16 first, quint16 second);
};

#endif // MODBUSCLIENT_H
