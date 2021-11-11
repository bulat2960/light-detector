#include "chart.h"
#include "pointlabelitem.h"

#include <QValueAxis>
#include <QGraphicsScene>

#include <cmath>

#include "logger.h"

static const QColor lightSeriesColor = QColor(192, 179, 0);
static const QColor smokeSeriesColor = Qt::blue;
static const QColor tgLineSeriesColor = Qt::red;
static const QColor maxDiffSeriesColor = Qt::green;
static const QColor linePenColor = Qt::black;
static const QColor titleColor = Qt::blue;

static const QString integerFormat = "%.0f";
static const QString floatFormat = "%.1f";

static const int initialTimeAxisValue = 10;

Chart::Chart()
{   
    setDropShadowEnabled(false);

    QPen widthPen;
    widthPen.setWidth(2);

    m_lightSeries = new QLineSeries(this);
    m_lightSeries->setPen(widthPen);
    m_lightSeries->setColor(lightSeriesColor);
    m_lightSeries->setName(QStringLiteral("Световой поток (I)"));

    m_smokeSeries = new QLineSeries(this);
    m_smokeSeries->setPen(widthPen);
    m_smokeSeries->setColor(smokeSeriesColor);
    m_smokeSeries->setName(QStringLiteral("Удельн. опт. плотн. дыма (Dуд)"));

    m_tgLineSeries = new QLineSeries(this);
    m_tgLineSeries->setPen(widthPen);
    m_tgLineSeries->setColor(tgLineSeriesColor);
    m_tgLineSeries->setName(QStringLiteral("Касательная"));
    m_tgLineSeries->hide();

    m_maxDiffSeries = new QSplineSeries(this);
    m_maxDiffSeries->setPen(widthPen);
    m_maxDiffSeries->setColor(maxDiffSeriesColor);
    m_maxDiffSeries->setName(QStringLiteral("Участок макс. приращ. Dуд"));
    m_maxDiffSeries->hide();

    addSeries(m_lightSeries);
    addSeries(m_smokeSeries);
    addSeries(m_maxDiffSeries);
    addSeries(m_tgLineSeries);

    setupAxes();
    setupTitle();
}

void Chart::setupTitle()
{
    QFont font;
    font.setPixelSize(25);

    setTitleBrush(titleColor);
    setTitleFont(font);
    setTitle(QStringLiteral("Световой поток и плотность дыма"));
}

void Chart::prepareToPrint()
{
    double ticks = ceil(m_lastTimestamp / 50);
    m_timeAxis->setRange(0, ticks * 50);
    m_timeAxis->setTickCount(ticks + 1);
    m_timeAxis->setLabelFormat(integerFormat);
}

void Chart::setupAxes()
{
    m_lightAxis = new QValueAxis(this);

    m_lightAxis->setTitleText(QStringLiteral("[Световой поток, I]"));
    m_lightAxis->setTitleBrush(lightSeriesColor);

    m_lightAxis->setLinePenColor(linePenColor);
    m_lightAxis->setLabelFormat(integerFormat + QStringLiteral("%"));

    m_lightAxis->setRange(0, 120);

    m_lightAxis->setTickCount(13);
    m_lightAxis->setMinorTickCount(4);

    m_lightAxis->setMinorGridLineVisible(false);
    m_lightAxis->setGridLineVisible(false);


    m_smokeAxis = new QValueAxis(this);

    m_smokeAxis->setTitleText(QStringLiteral("[Оптическая плотность, Dуд]"));
    m_smokeAxis->setTitleBrush(smokeSeriesColor);

    m_smokeAxis->setLinePenColor(linePenColor);
    m_smokeAxis->setLabelFormat(integerFormat);

    m_smokeAxis->setRange(0, 350);

    m_smokeAxis->setTickCount(8);

    m_smokeAxis->setMinorGridLineVisible(false);
    m_smokeAxis->setGridLineVisible(false);

    m_timeAxis = new QValueAxis(this);

    m_timeAxis->setTitleText(QStringLiteral("[Время (с), t]"));

    m_timeAxis->setLinePenColor(linePenColor);
    m_timeAxis->setLabelFormat(floatFormat);

    m_timeAxis->setRange(0, initialTimeAxisValue);

    m_timeAxis->setTickCount(11);
    m_timeAxis->setMinorTickCount(4);

    m_timeAxis->setMinorGridLineVisible(false);
    m_timeAxis->setGridLineVisible(false);


    addAxis(m_lightAxis, Qt::AlignLeft);
    addAxis(m_smokeAxis, Qt::AlignLeft);
    addAxis(m_timeAxis, Qt::AlignBottom);

    m_lightSeries->attachAxis(m_lightAxis);
    m_lightSeries->attachAxis(m_timeAxis);

    m_smokeSeries->attachAxis(m_smokeAxis);
    m_smokeSeries->attachAxis(m_timeAxis);

    m_tgLineSeries->attachAxis(m_smokeAxis);
    m_tgLineSeries->attachAxis(m_timeAxis);

    m_maxDiffSeries->attachAxis(m_smokeAxis);
    m_maxDiffSeries->attachAxis(m_timeAxis);
}

void Chart::setMaxLightValue(double value)
{
    m_maxLightValue = value;

    Logger::instance().logMessage(Logger::Type::CHART,
                                  QStringLiteral("Max light value set to %1")
                                  .arg(value));
}

void Chart::addPoint(double x, double y)
{
    m_lastTimestamp = x;

    if (m_isStick and x > m_timeAxis->max())
    {
        m_timeAxis->setRange(m_timeAxis->min(), m_lastTimestamp);
        changeTimeAxisTicksFormat();
    }

    double lightValue = y;
    double smokeValue = 132 * std::log10(m_maxLightValue / lightValue);

    double normalizedLightValue = (y / m_maxLightValue) * 100;

    m_lightSeries->append(x, ceil(normalizedLightValue * 100) / 100);
    m_smokeSeries->append(x, ceil(smokeValue * 100) / 100);

    m_experimentData.append({x, lightValue, smokeValue});

    emit dataChanged(normalizedLightValue, smokeValue);

    Logger::instance().logMessage(Logger::Type::CHART,
                                  QStringLiteral("Time: %1, Light: %2%, Smoke: %3")
                                  .arg(x)
                                  .arg(normalizedLightValue)
                                  .arg(smokeValue));
}

void Chart::addLabeledPoint(QPointF point, const QString& label)
{
    if (point.isNull())
    {
        return;
    }

    QPointF pos = mapToPosition(point, m_smokeSeries);

    PointLabelItem* item = new PointLabelItem(label);
    item->setPos(pos);
    scene()->addItem(item);

    LabeledPoint labeledPoint(item, point);
    labeledPoints.push_back(labeledPoint);

    Logger::instance().logMessage(Logger::Type::CHART,
                                  QStringLiteral("Labeled point: %1 at (%2, %3)")
                                  .arg(label)
                                  .arg(point.x())
                                  .arg(point.y()));
}

void Chart::updateLabeledPoints()
{
    for (LabeledPoint labeledPoint : labeledPoints)
    {
        QPointF point = labeledPoint.point;
        QPointF pos = mapToPosition(point, m_smokeSeries);
        labeledPoint.item->setPos(pos);
    }
}

QVector<DataUnit> Chart::experimentData() const
{
    return m_experimentData;
}

void Chart::changeTimeAxisTicksFormat()
{
    bool axisLengthLessThan50 = (m_timeAxis->max() - m_timeAxis->min()) < 50;
    m_timeAxis->setLabelFormat(axisLengthLessThan50 ? floatFormat : integerFormat);
}

double Chart::calculateTgA(int pointIndex, int sampleFrequency)
{
    int seconds = 2;
    int samplesInSeconds = sampleFrequency * seconds;

    QPointF startPoint = {m_experimentData[pointIndex - samplesInSeconds].timestamp,
                          m_experimentData[pointIndex - samplesInSeconds].smokeValue};

    QPointF endPoint = {m_experimentData[pointIndex].timestamp,
                        m_experimentData[pointIndex].smokeValue};

    QLineSeries series;
    series.append(startPoint);
    series.append(endPoint);

    bool ok;
    QPair<qreal, qreal> bestFit = series.bestFitLineEquation(ok);
    double tgA = bestFit.first;

    return tgA;
}

double Chart::calculateAndPlotTgData(int pointIndex, int lengthInSeconds, int sampleFrequency)
{
    int lengthInSamples = lengthInSeconds * sampleFrequency;

    if (m_experimentData.size() < pointIndex)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }

    DataUnit dataUnit = m_experimentData[pointIndex];

    double timestamp = dataUnit.timestamp;
    double smokeValue = dataUnit.smokeValue;

    double tgA = calculateTgA(pointIndex, sampleFrequency);

    QPointF lineEndPos = QPointF(timestamp, smokeValue);
    addLabeledPoint(lineEndPos, QStringLiteral("TgA = %1").arg(tgA));

    if (pointIndex > lengthInSamples)
    {
        double lineStartPosTimestamp = m_experimentData[pointIndex - lengthInSamples].timestamp;
        double y = tgA * (lineStartPosTimestamp - timestamp) + smokeValue;
        QPointF lineStartPos = {lineStartPosTimestamp, y};

        m_tgLineSeries->append({lineStartPos, lineEndPos});
        m_tgLineSeries->show();


        DataUnit startDataUnit = m_experimentData[pointIndex - lengthInSamples];
        DataUnit middleDataUnit = m_experimentData[pointIndex - lengthInSamples / 2];

        QPointF splineStartPos = {startDataUnit.timestamp, startDataUnit.smokeValue};
        QPointF splineMiddlePos = {middleDataUnit.timestamp, middleDataUnit.smokeValue};
        QPointF splineEndPos = lineEndPos;

        m_maxDiffSeries->append({splineStartPos, splineMiddlePos, splineEndPos});
        m_maxDiffSeries->show();
    }

    return tgA;
}

void Chart::scrollLeft()
{
    if (m_timeAxis->min() > 0)
    {
        double scrollValue = (m_timeAxis->max() - m_timeAxis->min()) / 300;

        if (m_timeAxis->min() < scrollValue)
        {
            scrollValue = m_timeAxis->min();
        }

        m_timeAxis->setRange(m_timeAxis->min() - scrollValue, m_timeAxis->max() - scrollValue);
        changeTimeAxisTicksFormat();
    }

    m_isStick = false;
    emit trackStateChanged(m_isStick);
}

void Chart::scrollRight()
{
    if (m_timeAxis->max() < m_lastTimestamp)
    {
        double scrollValue = (m_timeAxis->max() - m_timeAxis->min()) / 300;
        m_timeAxis->setRange(m_timeAxis->min() + scrollValue, m_timeAxis->max() + scrollValue);
        changeTimeAxisTicksFormat();
    }

    m_isStick = m_timeAxis->max() >= m_lastTimestamp;
    emit trackStateChanged(m_isStick);
}

void Chart::zoomInChart()
{
    // If size < 12
    if (m_timeAxis->max() <= m_timeAxis->min() + initialTimeAxisValue + 2)
    {
        return;
    }

    m_timeAxis->setRange(m_timeAxis->min() + 1, m_timeAxis->max() - 1);
    changeTimeAxisTicksFormat();

    m_isStick = false;
    emit trackStateChanged(m_isStick);
}

void Chart::zoomOutChart()
{   
    // Less than one -> 0, more than one -> -1
    m_timeAxis->setMin(m_timeAxis->min() > 1 ? m_timeAxis->min() - 1 : 0);

    // More than timestamp -> nothing, less -> +1
    m_timeAxis->setMax(m_timeAxis->max() < m_lastTimestamp ? m_timeAxis->max() + 1 : m_timeAxis->max());

    changeTimeAxisTicksFormat();

    m_isStick = m_timeAxis->max() >= m_lastTimestamp;
    emit trackStateChanged(m_isStick);
}
