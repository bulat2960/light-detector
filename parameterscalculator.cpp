#include "parameterscalculator.h"

#include "logger.h"

ParametersCalculator::ParametersCalculator(const QVector<DataUnit>& experimentData, double maxLightValue)
    : m_experimentData(experimentData),
      m_maxLightValue(maxLightValue)
{   

}

void ParametersCalculator::analyzeLightData()
{
    double Imin = std::numeric_limits<double>::max();
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

        if (lightValue < Imin) // Min light value <=> Max smoke value
        {
            Imin = lightValue;
            m_dMaxPoint = {timestamp, smokeValue};
        }
    }

    m_calculatedParameters.dMax = 132 * std::log10(m_maxLightValue / Imin);
    m_calculatedParameters.dt2 = 132 * std::log10(m_maxLightValue / It2);
    m_calculatedParameters.dt4 = 132 * std::log10(m_maxLightValue / It4);

    Logger::instance().logMessage(Logger::Type::CALCULATOR,
                                  QStringLiteral("DMax = %1").arg(m_calculatedParameters.dMax));

    Logger::instance().logMessage(Logger::Type::CALCULATOR,
                                  QStringLiteral("Dt2 = %1").arg(m_calculatedParameters.dt2));

    Logger::instance().logMessage(Logger::Type::CALCULATOR,
                                  QStringLiteral("Dt4 = %1").arg(m_calculatedParameters.dt4));
}

void ParametersCalculator::analyzeSmokeData(int samplingFrequency)
{
    double minDistanceToD16 = std::numeric_limits<double>::max();

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
            m_calculatedParameters.d16 = timestamp;
            m_d16Point = {timestamp, smokeValue};
        }

        for (int j = 0; j < DmTimestamps.size(); j++)
        {
            if (smokeValue < coefs[j] * m_calculatedParameters.dMax)
            {
                DmTimestamps[j] = timestamp;
            }
        }

        int samplesInMinute = 60 * samplingFrequency;

        bool experimentLengthMinuteOrMore = i >= samplesInMinute;
        bool samplesCountEnoughForMinute = m_experimentData.size() >= samplesInMinute;

        if (experimentLengthMinuteOrMore and samplesCountEnoughForMinute)
        {
            double minuteSmokeDiff = smokeValue - m_experimentData[i - samplesInMinute].smokeValue;

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

    m_calculatedParameters.Kcp = 0.2 * m_calculatedParameters.dMax * timeSum / 4;
    m_calculatedParameters.Unp = m_calculatedParameters.dMax * m_calculatedParameters.Kcp / (100 * m_calculatedParameters.d16);


    Logger::instance().logMessage(Logger::Type::CALCULATOR,
                                  QStringLiteral("D16 = %1").arg(m_calculatedParameters.d16));

    Logger::instance().logMessage(Logger::Type::CALCULATOR,
                                  QStringLiteral("Kcp = %1").arg(m_calculatedParameters.Kcp));

    Logger::instance().logMessage(Logger::Type::CALCULATOR,
                                  QStringLiteral("Unp = %1").arg(m_calculatedParameters.Unp));
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

CalculatedParameters ParametersCalculator::getCalculatedParameters() const
{
    return m_calculatedParameters;
}
