#ifndef PROTOCOLPARAMETERS_H
#define PROTOCOLPARAMETERS_H

#include <QString>

struct ProtocolParameters
{
    QString name {"-"};
    QString brand {"-"};
    QString provider {"-"};
    double thickness {0};

    int protocolNumber {0};
    double density {0};
    QString fio {"-"};
    int samplesNumber {0};
    QString conditioning {"-"};
    QString regime {"-"};

    double dMax {0};
    double dt2 {0};
    double dt4 {0};
    double d16 {0};
    double tgA {0};
    double Kcp {0};
    double Unp {0};
};

#endif // PROTOCOLPARAMETERS_H
