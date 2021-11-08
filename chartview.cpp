#include "chartview.h"

ChartView::ChartView(Chart *chart, QWidget *parent)
    : QChartView(chart, parent),
      m_chart(chart)
{
    connect(m_chart, &Chart::trackStateChanged, this, &ChartView::changeTrackLabel);
    connect(m_chart, &Chart::dataChanged, this, &ChartView::changeDataLabels);

    setRenderHint(QPainter::RenderHint::Antialiasing);
    setRenderHint(QPainter::RenderHint::TextAntialiasing);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    m_trackStateLabel = new FadingLabel(QStringLiteral("Режим отслеживания включен"),
                                        QStringLiteral("Режим отслеживания выключен"),
                                        this);

    QFont font = m_trackStateLabel->font();
    font.setBold(true);
    font.setPixelSize(14);
    m_trackStateLabel->setFont(font);

    m_lightValueLabel = new QLabel("I = None", this);
    m_smokeValueLabel = new QLabel("Dуд = None", this);

    font = m_lightValueLabel->font();
    font.setPixelSize(14);
    font.setBold(true);
    m_lightValueLabel->setFont(font);
    m_smokeValueLabel->setFont(font);
}

void ChartView::changeDataLabels(double lightValue, double smokeValue)
{
    m_lightValueLabel->setText(QStringLiteral("I = %1%").arg(ceil(lightValue * 100) / 100));
    m_smokeValueLabel->setText(QStringLiteral("Dуд = %1").arg(ceil(smokeValue * 100) / 100));
}

void ChartView::changeTrackLabel(bool state)
{
    m_trackStateLabel->changeState(state);
}

void ChartView::setTrackStateLabelGeometry(const QRect& geometry)
{
    m_trackStateLabel->setGeometry(geometry);
}

void ChartView::setDataLabelsGeometry(int left, int top)
{
    m_lightValueLabel->setGeometry(left, top, 200, 25);
    m_smokeValueLabel->setGeometry(left, top + 25, 200, 25);
}

void ChartView::hideLabels()
{
    m_trackStateLabel->hide();
    m_lightValueLabel->hide();
    m_smokeValueLabel->hide();
}

void ChartView::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
    {
        m_chart->zoomInChart();
    }
    else
    {
        m_chart->zoomOutChart();
    }
}

void ChartView::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPointF pos = event->pos();
        QPointF delta = pos - m_previousPos;

        if (delta.x() > 0)
        {
            m_chart->scrollLeft();
        }
        else
        {
            m_chart->scrollRight();
        }

        m_previousPos = pos;
    }

    QChartView::mouseMoveEvent(event);
}
