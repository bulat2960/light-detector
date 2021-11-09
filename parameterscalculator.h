#ifndef PARAMETERSCALCULATOR_H
#define PARAMETERSCALCULATOR_H

#include <QVector>
#include <QPointF>

#include "protocolcreator.h"
#include "dataunit.h"
#include "calculatedparameters.h"

class ParametersCalculator
{
public:
    ParametersCalculator(const QVector<DataUnit>& experimentData, double maxLightValue);

    void calculate(int samplingFrequency);

    CalculatedParameters getCalculatedParameters() const;

    int tgMaxPointIndex() const;

    QPointF dMaxPoint() const;
    QPointF dt2Point() const;
    QPointF dt4Point() const;
    QPointF d16Point() const;

private:
    CalculatedParameters m_calculatedParameters;

    QVector<DataUnit> m_experimentData;

    double m_maxLightValue;

    int m_tgMaxPointIndex;

    QPointF m_dMaxPoint;
    QPointF m_dt2Point;
    QPointF m_dt4Point;
    QPointF m_d16Point;

    void analyzeLightData();
    void analyzeSmokeData(int samplingFrequency);
};

#endif // PARAMETERSCALCULATOR_H
