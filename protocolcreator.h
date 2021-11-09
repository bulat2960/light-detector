#ifndef PROTOCOLCREATOR_H
#define PROTOCOLCREATOR_H

#include <QString>
#include <QPixmap>
#include <QObject>

#include "protocolparameters.h"
#include "calculatedparameters.h"

class ProtocolCreator : public QObject
{
    Q_OBJECT
public:
    ProtocolCreator() = default;

    void setProtocolParameters(const ProtocolParameters& protocolParameters);
    void setCalculatedParameters(const CalculatedParameters& calculatedParameters);
    void setTgA(double tgA);

    void setProtocolPath(const QString& path);
    void setGraph(const QPixmap& graph);

public slots:
    void createProtocol();

signals:
    void created();

private:
    ProtocolParameters m_protocolParameters;
    CalculatedParameters m_calculatedParameters;

    double m_tgA {0};

    QString m_path;
    QPixmap m_graph;

    QString convertTemplateToHtml(const QString& templateFilename);
};

#endif // PROTOCOLCREATOR_H
