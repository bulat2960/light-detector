#ifndef PARAMETERSCALCULATOR_H
#define PARAMETERSCALCULATOR_H

#include <QVector>
#include <QPointF>

#include "protocolcreator.h"
#include "dataunit.h"

class ParametersCalculator
{
public:
    ParametersCalculator(QVector<DataUnit> experimentData, double maxLightValue);

    void calculate(int samplingFrequency);

    void setParametersToProtocol(ProtocolCreator& creator);

    int tgMaxPointIndex() const;

    QPointF dMaxPoint() const;
    QPointF dt2Point() const;
    QPointF dt4Point() const;
    QPointF d16Point() const;

private:
    QVector<DataUnit> m_experimentData;

    double m_maxLightValue;

    double m_dMax {0};
    double m_dt2 {0};
    double m_dt4 {0};
    double m_d16 {0};
    double m_Kcp {0};
    double m_Unp {0};

    int m_tgMaxPointIndex;

    QPointF m_dMaxPoint;
    QPointF m_dt2Point;
    QPointF m_dt4Point;
    QPointF m_d16Point;

    void analyzeLightData();
    void analyzeSmokeData(int samplingFrequency);
};

#endif // PARAMETERSCALCULATOR_H
