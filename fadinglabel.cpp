#include "fadinglabel.h"

FadingLabel::FadingLabel(const QString &firstStateText, const QString &secondStateText, QWidget *parent)
    : QLabel(firstStateText, parent)
{
    m_firstStateText = firstStateText;
    m_secondStateText = secondStateText;

    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityEffect->setOpacity(0);
    setGraphicsEffect(m_opacityEffect);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(500);

    m_animHide = new QPropertyAnimation(m_opacityEffect, "opacity");
    m_animHide->setDuration(500);
    m_animHide->setStartValue(1.0);
    m_animHide->setEndValue(0.0);
    m_animHide->setEasingCurve(QEasingCurve::OutQuad);

    m_animShow = new QPropertyAnimation(m_opacityEffect, "opacity");
    m_animShow->setDuration(500);
    m_animShow->setStartValue(0.0);
    m_animShow->setEndValue(1.0);
    m_animShow->setEasingCurve(QEasingCurve::InQuad);

    connect(m_animShow, &QPropertyAnimation::finished, m_timer, QOverload<>::of(&QTimer::start));
    connect(m_timer, &QTimer::timeout, m_animHide, [this]
    {
        m_animHide->start();
    });
}

void FadingLabel::changeState(bool state)
{
    if (state != m_currentState)
    {
        QString stateString = state == true ? m_firstStateText : m_secondStateText;

        setText(stateString);

        m_currentState = state;

        m_animHide->stop();
        m_animShow->stop();
        m_timer->stop();

        m_animShow->start();
    }
}
