#include "protocolcreator.h"

#include <QPrinter>
#include <QPrinterInfo>
#include <QTextDocument>
#include <QPageLayout>
#include <QFile>
#include <QLocale>
#include <QDateTime>
#include <QTextCursor>
#include <QElapsedTimer>

#include "logger.h"

ProtocolCreator::ProtocolCreator()
{

}

QString ProtocolCreator::convertTemplateToHtml(const QString &templateFilename)
{
    QFile templateFile(templateFilename);

    if (not templateFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "File not open";
        return QString();
    }

    QByteArray templateString = templateFile.readAll();
    templateFile.close();

    QString resultHtml(templateString);

    resultHtml = resultHtml.arg(m_protocolNumber);

    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentDateTimeString = QLocale::system().toString(currentDateTime, QStringLiteral("ddd MMM yyyy hh:mm:ss"));

    resultHtml = resultHtml.arg(currentDateTimeString);

    resultHtml = resultHtml.arg(m_name);
    resultHtml = resultHtml.arg(m_brand);
    resultHtml = resultHtml.arg(m_thickness);
    resultHtml = resultHtml.arg(m_provider);

    resultHtml = resultHtml.arg(m_conditioning);
    resultHtml = resultHtml.arg(m_regime);
    resultHtml = resultHtml.arg(m_density);
    resultHtml = resultHtml.arg(m_samplesNumber);

    resultHtml = resultHtml.arg(m_dMax);
    resultHtml = resultHtml.arg(m_dt2);
    resultHtml = resultHtml.arg(m_dt4);
    resultHtml = resultHtml.arg(m_d16);
    resultHtml = resultHtml.arg(m_tgA);
    resultHtml = resultHtml.arg(m_Kcp);
    resultHtml = resultHtml.arg(m_Unp);

    resultHtml = resultHtml.arg(m_fio);

    return resultHtml;
}

void ProtocolCreator::setProtocolPath(const QString& path)
{
    m_path = path;

    Logger::instance().logMessage(Logger::Type::PROTOCOL,
                                  QStringLiteral("Protocol path set to %1").arg(path));
}

void ProtocolCreator::createProtocol()
{
    QElapsedTimer timer;
    timer.start();

    QPrinter printer(QPrinter::HighResolution);
    printer.setPrinterName("Microsoft Print to PDF");
    printer.setOutputFileName(m_path + QStringLiteral("/protocol-%1.pdf").arg(m_protocolNumber));
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize::A4);
    printer.setPageOrientation(QPageLayout::Portrait);

    QString htmlString = convertTemplateToHtml(":/templates/procotol.html");

    QSizeF pageSize = printer.pageRect(QPrinter::Unit::DevicePixel).size();

    QTextDocument doc;
    doc.setPageSize(pageSize);
    doc.setDefaultFont(QFont("Sans Serif", 138));

    QTextCursor cursor(&doc);

    cursor.insertHtml(htmlString);

    QImage image = m_graph.toImage();
    image = image.scaled(1360, 910, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    image.setDevicePixelRatio(0.1);

    QTransform transform;
    transform.rotate(270);
    image = image.transformed(transform, Qt::TransformationMode::SmoothTransformation);

    cursor.insertImage(image);

    doc.print(&printer);

    emit created();

    Logger::instance().logMessage(Logger::Type::PROTOCOL,
                                  QStringLiteral("Protocol created in %1 seconds").arg(timer.elapsed() / 1000.0));
}

void ProtocolCreator::setName(const QString& name)
{
    if (name.isEmpty())
    {
        m_name = "-";
        return;
    }

    m_name = name;

}
void ProtocolCreator::setBrand(const QString& brand)
{
    if (brand.isEmpty())
    {
        m_brand = "-";
        return;
    }

    m_brand = brand;
}

void ProtocolCreator::setProvider(const QString& provider)
{
    if (provider.isEmpty())
    {
        m_provider = "-";
        return;
    }

    m_provider = provider;
}

void ProtocolCreator::setThickness(double thickness)
{
    m_thickness = thickness;
}

void ProtocolCreator::setProtocolNumber(int protocolNumber)
{
    m_protocolNumber = protocolNumber;
}

void ProtocolCreator::setDensity(double density)
{
    m_density = density;
}

void ProtocolCreator::setFio(const QString& fio)
{
    if (fio.isEmpty())
    {
        m_fio = "-";
        return;
    }

    m_fio = fio;
}

void ProtocolCreator::setSamplesNumber(int samplesNumber)
{
    m_samplesNumber = samplesNumber;
}

void ProtocolCreator::setConditioning(const QString& conditioning)
{
    if (conditioning.isEmpty())
    {
        m_conditioning = "-";
        return;
    }

    m_conditioning = conditioning;
}

void ProtocolCreator::setRegime(const QString& regime)
{
    if (regime.isEmpty())
    {
        m_regime = "-";
        return;
    }

    m_regime = regime;
}

void ProtocolCreator::setDMax(double dMax)
{
    m_dMax = dMax;
}

void ProtocolCreator::setDt2(double dt2)
{
    m_dt2 = dt2;
}

void ProtocolCreator::setDt4(double dt4)
{
    m_dt4 = dt4;
}

void ProtocolCreator::setD16(double d16)
{
    m_d16 = d16;
}

void ProtocolCreator::setTgA(double tgA)
{
    m_tgA = tgA;
}

void ProtocolCreator::setKcp(double Kcp)
{
    m_Kcp = Kcp;
}

void ProtocolCreator::setUnp(double Unp)
{
    m_Unp = Unp;
}

void ProtocolCreator::setGraph(const QPixmap& graph)
{
    m_graph = graph;
}
