#include "parameterscalculator.h"

#include "logger.h"

ParametersCalculator::ParametersCalculator(QVector<DataUnit> experimentData, double maxLightValue)
{   
    m_maxLightValue = maxLightValue;
    m_experimentData = experimentData;
}

void ParametersCalculator::analyzeLightData()
{
    double Imin = 999999;
    double It2 = 0;
    double It4 = 0;

    for (int i = 0; i < m_experimentData.size(); i++)
    {
        double timestamp = m_experimentData[i].timestamp;
        double lightValue = m_experimentData[i].lightValue;
        double smokeValue = m_experimentData[i].smokeValue;

        if (round(timestamp) == 60 * 2) // 2 minutes timestamp
        {
            It2 = lightValue;
            m_dt2Point = {timestamp, smokeValue};
        }

        if (round(timestamp) == 60 * 4) // 4 minutes timestamp
        {
            It4 = lightValue;
            m_dt4Point = {timestamp, smokeValue};
        }

        if (lightValue < Imin)
        {
            Imin = lightValue;
            m_dMaxPoint = {timestamp, smokeValue};
        }
    }

    m_dMax = 132 * std::log10(m_maxLightValue / Imin);
    m_dt2 = 132 * std::log10(m_maxLightValue / It2);
    m_dt4 = 132 * std::log10(m_maxLightValue / It4);

    Logger::instance().logMessage(Logger::Type::CALCULATOR,
                                  QStringLiteral("DMax = %1").arg(m_dMax));

    Logger::instance().logMessage(Logger::Type::CALCULATOR,
                                  QStringLiteral("Dt2 = %1").arg(m_dt2));

    Logger::instance().logMessage(Logger::Type::CALCULATOR,
                                  QStringLiteral("Dt4 = %1").arg(m_dt4));
}

void ParametersCalculator::analyzeSmokeData(int samplingFrequency)
{
    double minDistanceToD16 = 999999;

    QVector<double> DmTimestamps(5);
    const QVector<double> coefs = {0.1, 0.3, 0.5, 0.7, 0.9};

    int maxMinuteSmokeDiff = 0;

    for (int i = 0; i < m_experimentData.size(); i++)
    {
        double timestamp = m_experimentData[i].timestamp;
        double smokeValue = m_experimentData[i].smokeValue;

        double distanceToD16 = fabs(16 - smokeValue);

        if (distanceToD16 < minDistanceToD16)
        {
            minDistanceToD16 = distanceToD16;
            m_d16 = timestamp;
            m_d16Point = {timestamp, smokeValue};
        }

        for (int i = 0; i < DmTimestamps.size(); i++)
        {
            if (smokeValue < coefs[i] * m_dMax)
            {
                DmTimestamps[i] = timestamp;
            }
        }

        int samplesInMinute = 60 * samplingFrequency;

        if (m_experimentData.size() >= samplesInMinute and i >= samplesInMinute)
        {
            double minuteSmokeDiff = smokeValue - m_experimentData[i - 60 * samplingFrequency].smokeValue;

            if (minuteSmokeDiff > maxMinuteSmokeDiff)
            {
                maxMinuteSmokeDiff = minuteSmokeDiff;
                m_tgMaxPointIndex = i;
            }
        }
    }

    double timeSum = 0;
    for (int i = 0; i < DmTimestamps.size() - 1; i++)
    {
        timeSum = 1.0 / (DmTimestamps[i + 1] - DmTimestamps[i]);
    }
    m_Kcp = 0.2 * m_dMax * timeSum / 4;

    m_Unp = m_dMax * m_Kcp / (100 * m_d16);


    Logger::instance().logMessage(Logger::Type::CALCULATOR,
                                  QStringLiteral("D16 = %1").arg(m_d16));

    Logger::instance().logMessage(Logger::Type::CALCULATOR,
                                  QStringLiteral("Kcp = %1").arg(m_Kcp));

    Logger::instance().logMessage(Logger::Type::CALCULATOR,
                                  QStringLiteral("Unp = %1").arg(m_Unp));
}

void ParametersCalculator::calculate(int samplingFrequency)
{   
    analyzeLightData();
    analyzeSmokeData(samplingFrequency);
}

int ParametersCalculator::tgMaxPointIndex() const
{
    return m_tgMaxPointIndex;
}

QPointF ParametersCalculator::dMaxPoint() const
{
    return m_dMaxPoint;
}

QPointF ParametersCalculator::dt2Point() const
{
    return m_dt2Point;
}

QPointF ParametersCalculator::dt4Point() const
{
    return m_dt4Point;
}

QPointF ParametersCalculator::d16Point() const
{
    return m_d16Point;
}

void ParametersCalculator::setParametersToProtocol(ProtocolCreator& creator)
{
    creator.setDMax(m_dMax);
    creator.setDt2(m_dt2);
    creator.setDt4(m_dt4);
    creator.setD16(m_d16);
    creator.setKcp(m_Kcp);
    creator.setUnp(m_Unp);
}
