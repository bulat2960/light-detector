#ifndef FADINGLABEL_H
#define FADINGLABEL_H

#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>

class FadingLabel : public QLabel
{
public:
    FadingLabel(const QString& firstStateText, const QString& secondStateText, QWidget* parent = nullptr);

    void changeState(bool state);

private:
    QTimer* m_timer {nullptr};
    QGraphicsOpacityEffect* m_opacityEffect {nullptr};
    QPropertyAnimation* m_animHide {nullptr};
    QPropertyAnimation* m_animShow {nullptr};
    bool m_currentState {false};

    QString m_firstStateText;
    QString m_secondStateText;
};

#endif // FADINGLABEL_H
