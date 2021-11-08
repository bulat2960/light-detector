#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QChartView>
#include <QTimer>

#include "chart.h"
#include "fadinglabel.h"

class ChartView : public QChartView
{
    Q_OBJECT

public:
    ChartView(Chart* chart, QWidget* parent = 0);

    void setTrackStateLabelGeometry(const QRect& geometry);
    void setDataLabelsGeometry(int left, int top);
    void hideLabels();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void changeTrackLabel(bool state);
    void changeDataLabels(double lightValue, double smokeValue);

private:
    Chart* m_chart {nullptr};

    FadingLabel* m_trackStateLabel {nullptr};
    QLabel* m_lightValueLabel {nullptr};
    QLabel* m_smokeValueLabel {nullptr};
    bool m_currentState {false};

    QPointF m_previousPos;
};

#endif // CHARTVIEW_H
