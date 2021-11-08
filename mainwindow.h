#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QElapsedTimer>
#include <QTimer>
#include <QMessageBox>
#include <QMovie>
#include <QToolBar>

#include "chart.h"
#include "chartview.h"
#include "settingsdialog.h"
#include "modbusclient.h"
#include "protocolcreator.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QSize screenSize, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onExperimentStart();
    void onExperimentStop();
    void openLogFolderDialog();
    void calibrate();
    void showInfoAboutProgram();
    void startExperiment(double calibrationValue);
    void openSettings();
    void toggleLogging();

    void preparePdfGenerating();
    void generatePdf();

private:
    int m_serverAddress {1};
    double m_maxLightValue {std::numeric_limits<double>::quiet_NaN()};

    ProtocolCreator m_creator;

    QElapsedTimer m_experimentTimer;

    QTimer* m_requestTimer {nullptr};
    QMovie* m_movie {nullptr};

    ModbusClient* m_client {nullptr};

    QVBoxLayout* m_mainLayout {nullptr};

    QToolBar* m_toolBar {nullptr};

    QAction* m_connectAction {nullptr};
    QToolButton* m_startExperimentButton {nullptr};
    QAction* m_stopExperimentAction {nullptr};
    QAction* m_archiveAction {nullptr};
    QAction* m_calibrationAction {nullptr};
    QAction* m_aboutProgramAction {nullptr};
    QAction* m_loggingAction {nullptr};
    QAction* m_exitProgramAction {nullptr};

    ChartView* m_chartView {nullptr};
    Chart* m_chart {nullptr};

    void createToolBar();
    void createChartView();

    void addPointToChart(double value);

    QString getProtocolsFolder();
};
#endif // MAINWINDOW_H
