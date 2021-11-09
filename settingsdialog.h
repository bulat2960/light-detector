#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

#include "modbusclient.h"

class SettingsDialog : public QDialog
{
public:
    SettingsDialog(ModbusClient* client, QWidget* parent = 0);

private slots:
    void scanExistingPorts();
    void onConnectionEstablished();

private:
    QComboBox* m_portBox {nullptr};
    QComboBox* m_baudRateBox {nullptr};
    QComboBox* m_dataBitsBox {nullptr};
    QComboBox* m_parityBox {nullptr};
    QComboBox* m_stopBitsBox {nullptr};
    QComboBox* m_timeoutBox {nullptr};
    QLineEdit* m_addressLineEdit {nullptr};

    QLabel* m_portNameLabel {nullptr};
    QLabel* m_connectionEstablishedLabel {nullptr};

    QPushButton* m_openPortButton {nullptr};
    QPushButton* m_checkConnectionButton {nullptr};

    QPushButton* m_acceptButton {nullptr};
    QPushButton* m_rejectButton {nullptr};

    QVBoxLayout* m_mainLayout {nullptr};

    ModbusClient* m_client {nullptr};

    QMap<QString, QList<int>> portBaudRates;

    void createDataElementsGrid();
    void createPortElementsGrid();
    void createDialogButtonsLayout();

    void openPort();

    void changeBaudRates();
};

#endif // SETTINGSDIALOG_H
