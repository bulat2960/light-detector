#ifndef PROTOCOLPARAMETERS_H
#define PROTOCOLPARAMETERS_H

#include <QString>

struct ProtocolParameters
{
    QString materialBrand;
    QString batchNumber;
    QString recipe;
    QString weightAndThickness;

    QString testSeriesNumber;
    QString density;
    QString sampleType;
    QString firstNote;
    QString secondNote;
    QString regime;

    double dMax {0};
    double dt2 {0};
    double dt4 {0};
    double d16 {0};
    double tgA {0};
    double Kcp {0};
    double Unp {0};
};

#endif // PROTOCOLPARAMETERS_H
