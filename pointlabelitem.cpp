#include "pointlabelitem.h"

#include <QPainter>
#include <QRandomGenerator>

PointLabelItem::PointLabelItem(const QString &text)
{
    setZValue(10);

    m_text = text;
    m_randomHeight = QRandomGenerator::global()->bounded(20, 60);
}

QRectF PointLabelItem::boundingRect() const
{
    return QRectF(0, 0, 80, 200);
}

void PointLabelItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->translate(-40, -100);

    QPainterPath path;
    path.addEllipse(QRectF(36, 96, 8, 8));
    painter->fillPath(path, Qt::red);
    painter->drawEllipse(QRectF(36, 96, 8, 8));

    painter->drawText(QRectF(0, 92 - m_randomHeight - 15, 80, 15), Qt::AlignCenter, m_text);

    painter->drawLine(40, 92, 40, 92 - m_randomHeight);
    painter->drawLine(40, 92, 35, 87);
    painter->drawLine(40, 92, 45, 87);
}
