#ifndef CHART_H
#define CHART_H

#include <QChart>
#include <QSplineSeries>
#include <QLineSeries>
#include <QValueAxis>
#include <QLabel>
#include <QGraphicsSimpleTextItem>

#include "dataunit.h"
#include "pointlabelitem.h"

class Chart : public QChart
{
    Q_OBJECT

public:
    Chart(int samplesAfterReachMaxSmokeToNotify);

    void scrollLeft();
    void scrollRight();
    void zoomInChart();
    void zoomOutChart();

    void setMaxLightValue(double value);

    void prepareToPrint();

    void addLabeledPoint(QPointF point, const QString& label);
    double calculateAndPlotTgData(int pointIndex, int lengthInSeconds, int sampleFrequency);

    void updateLabeledPoints();

    QVector<DataUnit> experimentData() const;

signals:
    void trackStateChanged(bool state);
    void dataChanged(double lightValue, double smokeValue);
    void maxSmokeReached();

public slots:
    void addPoint(double x, double y);

private:
    bool m_isStick {true};

    double m_lastTimestamp {0.0};
    double m_lastLightValue {0.0};
    double m_lastSmokeValue {0.0};
    double m_maxLightValue{0.0};

    const int m_samplesAfterReachMaxSmokeToNotify;
    double m_maxReachedSmokeValue {0.0};
    int m_currentSamplesFromMaxSmoke {0};
    bool m_maxSmokeReached {false};

    QVector<DataUnit> m_experimentData;

    QLineSeries* m_lightSeries {nullptr};
    QLineSeries* m_smokeSeries {nullptr};
    QLineSeries* m_tgLineSeries {nullptr};
    QSplineSeries* m_maxDiffSeries {nullptr};

    QValueAxis* m_timeAxis {nullptr};
    QValueAxis* m_smokeAxis {nullptr};
    QValueAxis* m_lightAxis {nullptr};

    struct LabeledPoint
    {
        PointLabelItem* item {nullptr};
        QPointF point;

        LabeledPoint(PointLabelItem* item, QPointF point)
        {
            this->item = item;
            this->point = point;
        }
    };

    QVector<LabeledPoint> labeledPoints;

    void setupTitle();
    void setupAxes();

    void checkIfMaxSmokeReached(double normalizedLightValue, double smokeValue);

    void changeTimeAxisTicksFormat();

    double calculateTgA(int pointIndex, int sampleFrequency);
};

#endif // CHART_H
