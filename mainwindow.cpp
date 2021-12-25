#include <QToolBar>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QMovie>
#include <QToolButton>
#include <QThread>

#include "mainwindow.h"
#include "startexperimentdialog.h"
#include "parameterscalculator.h"
#include "logger.h"
#include "programsettingsdialog.h"

static const int samplingFrequency = 5;
static const int tgLineLengthInSeconds = 40;

MainWindow::MainWindow(QSize screenSize, QWidget *parent)
    : QWidget(parent)
{
    m_chart = new Chart;

    m_chart->legend()->detachFromChart();
    m_chart->legend()->setGeometry(QRectF(screenSize.width() - 250, 0, 250, 500));

    m_chartView = new ChartView(m_chart, this);
    m_chartView->setTrackStateLabelGeometry(QRect(screenSize.width() - 270, screenSize.height() - 200, 270, 100));
    m_chartView->setDataLabelsGeometry(screenSize.width() - 370, 10);

    m_client = new ModbusClient(samplingFrequency, this);
    connect(m_client, &ModbusClient::dataParsed, this, &MainWindow::addPointToChart);
    connect(m_client, &ModbusClient::calibrationValueReceived, this, &MainWindow::startExperiment);

    m_mainLayout = new QVBoxLayout(this);
    createToolBar();
    m_mainLayout->addWidget(m_chartView);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    m_chart->legend()->setGeometry(QRectF(size().width() - 250, 0, 250, 500));
    m_chart->updateLabeledPoints();
    m_chartView->setTrackStateLabelGeometry(QRect(size().width() - 270, size().height() - 200, 270, 100));
    m_chartView->setDataLabelsGeometry(size().width() - 370, 10);
    QWidget::resizeEvent(event);
}

void MainWindow::openConnectionSettings()
{
    ConnectionSettingsDialog dialog(m_client);
    int code = dialog.exec();

    if (code == QDialog::Rejected)
    {
        m_connectAction->setEnabled(true);
        m_startExperimentButton->setEnabled(false);
        m_calibrationAction->setEnabled(false);
        m_client->disconnectDevice();
    }
    else
    {
        m_connectAction->setEnabled(false);
        m_startExperimentButton->setEnabled(true);
        m_calibrationAction->setEnabled(true);
    }
}

void MainWindow::createToolBar()
{
    m_toolBar = new QToolBar(this);
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_toolBar->setIconSize(QSize(100, 60));

    m_connectAction = new QAction(QIcon(":/images/connect.png"), QStringLiteral("Подключение"), this);
    m_connectAction->setToolTip(QStringLiteral("Подключиться к COM-порту"));
    connect(m_connectAction, &QAction::triggered, this, &MainWindow::openConnectionSettings);

    m_startExperimentButton = new QToolButton(this);
    m_startExperimentButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_startExperimentButton->setIcon(QIcon(":/images/start.png"));
    m_startExperimentButton->setText(QStringLiteral("Старт"));
    m_startExperimentButton->setToolTip(QStringLiteral("Старт эксперимента"));
    m_startExperimentButton->setDisabled(true);
    connect(m_startExperimentButton, &QToolButton::clicked, this, &MainWindow::onExperimentStart);

    m_movie = new QMovie(":/images/processing.gif", "gif", this);
    connect(m_movie, &QMovie::finished, m_movie, &QMovie::start);
    connect(m_movie, &QMovie::frameChanged, this, [this] {
        m_startExperimentButton->setIcon(QIcon(m_movie->currentPixmap()));
    });

    m_stopExperimentAction = new QAction(QIcon(":/images/stop.png"), QStringLiteral("Стоп"), this);
    m_stopExperimentAction->setToolTip(QStringLiteral("Остановка эксперимента"));
    m_stopExperimentAction->setDisabled(true);
    connect(m_stopExperimentAction, &QAction::triggered, this, &MainWindow::onExperimentStop);

    m_archiveAction = new QAction(QIcon(":/images/archive.png"), QStringLiteral("Отчеты"), this);
    m_archiveAction->setToolTip(QStringLiteral("Открыть папку с отчетами"));
    connect(m_archiveAction, &QAction::triggered, this, &MainWindow::openLogFolderDialog);

    m_calibrationAction = new QAction(QIcon(":/images/calibrate.png"), QStringLiteral("Калибровка"), this);
    m_calibrationAction->setToolTip(QStringLiteral("Откалибровать устройство"));
    m_calibrationAction->setDisabled(true);
    connect(m_calibrationAction, &QAction::triggered, this, &MainWindow::calibrate);

    m_aboutProgramAction = new QAction(QIcon(":/images/about.png"), QStringLiteral("О программе"), this);
    m_aboutProgramAction->setToolTip(QStringLiteral("О программе"));
    connect(m_aboutProgramAction, &QAction::triggered, this, &MainWindow::showInfoAboutProgram);

    m_maxSmokeNotifyLabel = new FadingLabel(QStringLiteral("Достигнуто максимальное значение плотности дыма."),
                                            QStringLiteral("Можно завершать работу программы."), this);
    m_maxSmokeNotifyLabel->setWordWrap(true);
    m_maxSmokeNotifyLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_maxSmokeNotifyLabel->setAlignment(Qt::AlignCenter);
    m_maxSmokeNotifyLabel->setStyleSheet("QLabel {font-size: 15pt; }");
    m_maxSmokeNotifyLabel->setAnimationDuration(1000);
    m_maxSmokeNotifyLabel->setVisible(true);
    connect(m_chart, &Chart::maxSmokeReached, m_maxSmokeNotifyLabel, &FadingLabel::startCycled);

    m_settingsAction = new QAction(QIcon(":/images/settings.png"), QStringLiteral("Настройки"));
    m_settingsAction->setToolTip(QStringLiteral("Открыть настройки"));
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::openProgramSettings);

    m_loggingAction = new QAction(QIcon(":/images/log-disabled.png"), QStringLiteral("Лог выкл."));
    m_loggingAction->setToolTip(QStringLiteral("Включить запись логирования"));
    connect(m_loggingAction, &QAction::triggered, this, &MainWindow::toggleLogging);

    m_toolBar->addSeparator();
    m_toolBar->addAction(m_connectAction);
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_startExperimentButton);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_stopExperimentAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_archiveAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_calibrationAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_aboutProgramAction);
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_maxSmokeNotifyLabel);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_settingsAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_loggingAction);
    m_toolBar->addSeparator();

    m_mainLayout->addWidget(m_toolBar);
}

void MainWindow::openProgramSettings()
{
    ProgramSettingsDialog dialog(m_chart->samplesAfterReachMaxSmokeToNotify() / samplingFrequency, this);
    int code = dialog.exec();

    if (code == QDialog::Rejected)
    {
        return;
    }

    int maxSmokeTrackingDelaySeconds = dialog.getMaxSmokeTrackingDelaySeconds();
    m_chart->setSamplesAfterReachMaxSmokeToNotify(maxSmokeTrackingDelaySeconds * samplingFrequency);
}

void MainWindow::toggleLogging()
{
    bool isLoggingEnabled = Logger::instance().isLoggingEnabled();

    if (not isLoggingEnabled)
    {
        m_loggingAction->setIcon(QIcon(":/images/log-enabled.png"));
        m_loggingAction->setText(QStringLiteral("Лог вкл."));
        m_loggingAction->setToolTip(QStringLiteral("Выключить запись лога"));
    }
    else
    {
        m_loggingAction->setIcon(QIcon(":/images/log-disabled.png"));
        m_loggingAction->setText(QStringLiteral("Лог выкл."));
        m_loggingAction->setToolTip(QStringLiteral("Включить запись лога"));
    }

    Logger::instance().setLoggingEnabled(not isLoggingEnabled);
}

void MainWindow::addPointToChart(double value)
{
    if (not std::isfinite(value))
    {
        return;
    }

    if (not m_experimentTimer.isValid())
    {
        if (m_maxLightValue < 0 or not std::isfinite(m_maxLightValue))
        {
            return;
        }

        m_experimentTimer.start();
        m_chart->setMaxLightValue(m_maxLightValue);
    }

    double time = m_experimentTimer.elapsed() * 1.0 / 1000;
    m_chart->addPoint(time, value);
}

void MainWindow::onExperimentStart()
{
    StartExperimentDialog dialog(this);
    int code = dialog.exec();

    if (code == QDialog::Rejected)
    {
        return;
    }

    m_creator.setProtocolParameters(dialog.getProtocolParameters());
    m_client->sendCalibrationRequest();
}

void MainWindow::startExperiment(double calibrationValue)
{
    m_maxLightValue = calibrationValue;

    if (m_maxLightValue <= 0)
    {
        QMessageBox box;
        box.setIcon(QMessageBox::Icon::Warning);
        box.setWindowTitle(QStringLiteral("Ошибка"));
        box.setTextFormat(Qt::RichText);
        box.setText(QStringLiteral("Не удалось получить начальное значение.<br>"
                                   "Пожалуйста, проверьте устройство или перезапустите программу."));
        box.exec();
    }
    else
    {
        m_client->start();

        m_movie->start();

        m_startExperimentButton->setEnabled(false);
        m_stopExperimentAction->setEnabled(true);
        m_calibrationAction->setEnabled(false);
        m_loggingAction->setEnabled(false);
        m_settingsAction->setEnabled(false);
        m_startExperimentButton->setText(QStringLiteral("В процессе"));

        Logger::instance().logMessage(Logger::Type::EXPERIMENT,
                                      QStringLiteral("Experiment started"));
    }
}

void MainWindow::onExperimentStop()
{
    if (m_client->isActive())
    {
        m_movie->stop();
        m_stopExperimentAction->setDisabled(true);
        m_startExperimentButton->setText(QStringLiteral("Остановлено"));
        m_calibrationAction->setEnabled(false);
        m_client->stop();
        m_chartView->hideLabels();
        m_chartView->setDisabled(true);

        disconnect(m_client, &ModbusClient::dataParsed, this, &MainWindow::addPointToChart);

        Logger::instance().logMessage(Logger::Type::EXPERIMENT,
                                      QStringLiteral("Experiment stopped"));
    }

    preparePdfGenerating();
}

QString MainWindow::getProtocolsFolder()
{
    QString documentsFolderPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QStringLiteral("\\Documents");
    QString protocolsFolderPath = QStringLiteral("%1/%2").arg(documentsFolderPath, "LightDetector");

    QDir dir(protocolsFolderPath);
    if (not dir.exists())
    {
        dir.mkpath(".");
    }

    return dir.path();
}

void MainWindow::openLogFolderDialog()
{
    QString protocolsFolderPath = getProtocolsFolder();
    QDesktopServices::openUrl(QUrl::fromLocalFile(protocolsFolderPath));
}

void MainWindow::calibrate()
{
    // TODO: Add calibration dialog
}

void MainWindow::showInfoAboutProgram()
{
    QString aboutText = QStringLiteral(
                "<h1>Quantum Polysmoke v1.0.0</h1>"
                "<p align=\"justify\" style=\"font-size: 15px\">"
                "Установка для оценки дымообразования полимерных материалов\n"
                "измеряет оптическую плотность дыма, производимого материалами\n"
                "при нагревании образца размерами 75х75мм и толщиной до 15±0.5мм\n"
                "источником теплового потока мощностью не более 800 Вт с обеспечением\n"
                "теплового потока на образце 4 Вт/см<sup>2</sup> в замкнутой камере с\n"
                "использованием газовой горелки в двух режимах: пламенного горения и тления."
                "</p>");

    QMessageBox box;
    box.setIcon(QMessageBox::Icon::Information);
    box.setWindowTitle(QStringLiteral("О программе"));
    box.setText(aboutText);
    box.setTextFormat(Qt::RichText);

    QPushButton* aboutQtButton = box.addButton(QStringLiteral("Информация о Qt"), QMessageBox::YesRole);
    box.setStandardButtons(QMessageBox::StandardButton::Ok);

    box.exec();

    if (box.clickedButton() == aboutQtButton)
    {
        QMessageBox::aboutQt(this);
    }
}

void MainWindow::preparePdfGenerating()
{
    QVector<DataUnit> experimentData = m_chart->experimentData();
    ParametersCalculator calculator(experimentData, m_maxLightValue);
    calculator.calculate(samplingFrequency);
    m_creator.setCalculatedParameters(calculator.getCalculatedParameters());

    m_chart->prepareToPrint();

    QPointF d16Point = calculator.d16Point();
    double d16Value = round(d16Point.x());
    QPointF dMaxPoint = calculator.dMaxPoint();
    double dMaxValue = ceil(dMaxPoint.y() * 100) / 100;
    QPointF dt2Point = calculator.dt2Point();
    double dt2Value = ceil(dt2Point.y() * 100) / 100;
    QPointF dt4Point = calculator.dt4Point();
    double dt4Value = ceil(dt4Point.y() * 100) / 100;

    m_chart->addLabeledPoint(d16Point, QStringLiteral("D16 - %1c").arg(d16Value));
    m_chart->addLabeledPoint(dMaxPoint, QStringLiteral("Dmax = %1").arg(dMaxValue));
    m_chart->addLabeledPoint(dt2Point, QStringLiteral("Dt2 = %1").arg(dt2Value));
    m_chart->addLabeledPoint(dt4Point, QStringLiteral("Dt4 = %1").arg(dt4Value));

    int tgAmaxPointIndex = calculator.tgMaxPointIndex();

    double tgA = m_chart->calculateAndPlotTgData(tgAmaxPointIndex, tgLineLengthInSeconds, samplingFrequency);
    m_creator.setTgA(tgA);

    generatePdf();
}

void MainWindow::generatePdf()
{
    QMessageBox box;
    box.setIcon(QMessageBox::Icon::Information);
    box.setWindowTitle(QStringLiteral("Информация"));
    box.setText(QStringLiteral("Ожидайте, производится создание протокола..."));
    box.setStandardButtons(QMessageBox::NoButton);

    QThread* thread = new QThread(this);

    QTimer::singleShot(1000, this, [this, thread] {
        m_creator.setGraph(m_chartView->grab());
        QString protocolsFolderPath = getProtocolsFolder();
        m_creator.setProtocolPath(protocolsFolderPath);

        m_creator.moveToThread(thread);
        thread->start();
    });

    connect(thread, &QThread::started, &m_creator, &ProtocolCreator::createProtocol);
    connect(&m_creator, &ProtocolCreator::created, &box, &QMessageBox::accept);
    connect(&m_creator, &ProtocolCreator::created, thread, [thread, this] {
        thread->quit();
        thread->wait(1000);
        m_startExperimentButton->setIcon(QIcon(":/images/start.png"));
        m_startExperimentButton->setText(QStringLiteral("Завершено"));
    });

    box.exec();
}

MainWindow::~MainWindow()
{
    m_movie->stop();
    m_client->stop();
    m_client->disconnectDevice();
}
