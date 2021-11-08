#ifndef POINTLABELITEM_H
#define POINTLABELITEM_H

#include <QGraphicsObject>

class PointLabelItem : public QGraphicsObject
{
    Q_OBJECT

public:
    PointLabelItem(const QString& text);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    QString m_text;
    int m_randomHeight;
};

#endif // POINTLABELITEM_H
