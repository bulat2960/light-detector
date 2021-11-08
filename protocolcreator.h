#ifndef PROTOCOLCREATOR_H
#define PROTOCOLCREATOR_H

#include <QString>
#include <QPixmap>
#include <QObject>

class ProtocolCreator : public QObject
{
    Q_OBJECT
public:
    ProtocolCreator();

    void setProtocolPath(const QString& path);

    void setName(const QString& name);
    void setBrand(const QString& brand);
    void setProvider(const QString& provider);
    void setThickness(double thickness);

    void setProtocolNumber(int protocolNumber);
    void setDensity(double density);
    void setFio(const QString& fio);
    void setSamplesNumber(int samplesNumber);
    void setConditioning(const QString& conditioning);
    void setRegime(const QString& regime);

    void setDMax(double dMax);
    void setDt2(double dt2);
    void setDt4(double dt4);
    void setD16(double d16);
    void setTgA(double tgA);
    void setKcp(double Kcp);
    void setUnp(double Unp);

    void setGraph(const QPixmap& graph);

public slots:
    void createProtocol();

signals:
    void created();

private:
    QString m_name {"-"};
    QString m_brand {"-"};
    QString m_provider {"-"};
    double m_thickness {0};

    int m_protocolNumber {0};
    double m_density {0};
    QString m_fio {"-"};
    int m_samplesNumber {0};
    QString m_conditioning {"-"};
    QString m_regime {"-"};

    double m_dMax {0};
    double m_dt2 {0};
    double m_dt4 {0};
    double m_d16 {0};
    double m_tgA {0};
    double m_Kcp {0};
    double m_Unp {0};

    QString m_path;
    QPixmap m_graph;

    QString convertTemplateToHtml(const QString& templateFilename);
};

#endif // PROTOCOLCREATOR_H
