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

    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentDateTimeString = QLocale::system().toString(currentDateTime, QStringLiteral("ddd MMM yyyy hh:mm:ss"));

    resultHtml = resultHtml.arg(m_protocolParameters.testSeriesNumber);
    resultHtml = resultHtml.arg(currentDateTimeString);

    resultHtml = resultHtml.arg(m_protocolParameters.materialBrand);
    resultHtml = resultHtml.arg(m_protocolParameters.batchNumber);
    resultHtml = resultHtml.arg(m_protocolParameters.weightAndThickness);
    resultHtml = resultHtml.arg(m_protocolParameters.recipe);
    resultHtml = resultHtml.arg(m_protocolParameters.sampleType);

    resultHtml = resultHtml.arg(m_protocolParameters.regime);
    resultHtml = resultHtml.arg(m_protocolParameters.density);
    resultHtml = resultHtml.arg(m_protocolParameters.firstNote);
    resultHtml = resultHtml.arg(m_protocolParameters.secondNote);

    resultHtml = resultHtml.arg(m_calculatedParameters.dMax);
    resultHtml = resultHtml.arg(m_calculatedParameters.dt2);
    resultHtml = resultHtml.arg(m_calculatedParameters.dt4);
    resultHtml = resultHtml.arg(m_calculatedParameters.d16);
    resultHtml = resultHtml.arg(m_tgA);
    resultHtml = resultHtml.arg(m_calculatedParameters.Kcp);
    resultHtml = resultHtml.arg(m_calculatedParameters.Unp);

    return resultHtml;
}

void ProtocolCreator::setProtocolParameters(const ProtocolParameters &protocolParameters)
{
    m_protocolParameters = protocolParameters;
}

void ProtocolCreator::setCalculatedParameters(const CalculatedParameters& calculatedParameters)
{
    m_calculatedParameters = calculatedParameters;
}

void ProtocolCreator::setTgA(double tgA)
{
    m_tgA = tgA;
}

void ProtocolCreator::setProtocolPath(const QString& path)
{
    m_path = path;

    Logger::instance().logMessage(Logger::Type::PROTOCOL,
                                  QStringLiteral("Protocol path set to %1").arg(path));
}

void ProtocolCreator::setGraph(const QPixmap& graph)
{
    m_graph = graph;
}

void ProtocolCreator::createProtocol()
{
    QElapsedTimer timer;
    timer.start();

    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentDateTimeString = currentDateTime.toString(QStringLiteral("dd-MM-yyyy"));

    QString protocolName = QStringLiteral("%1_%2_%3_%4.pdf").arg(currentDateTimeString)
                                                        .arg(m_protocolParameters.materialBrand)
                                                        .arg(m_protocolParameters.batchNumber)
                                                        .arg(m_protocolParameters.testSeriesNumber);

    QString protocolPath = QStringLiteral("%1/%2").arg(m_path).arg(protocolName);

    /*QPrinter printer(QPrinter::HighResolution);
    printer.setPrinterName("Microsoft Print to PDF");
    printer.setOutputFileName(protocolPath);
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

    doc.print(&printer);*/


    QPrinter printer(QPrinter::HighResolution);
    printer.setPrinterName("Microsoft Print to PDF");
    printer.setOutputFileName(protocolPath);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize::A4);
    printer.setPageOrientation(QPageLayout::Landscape);
    QSizeF pageSize = printer.pageRect(QPrinter::Unit::DevicePixel).size();

    QTextDocument doc;
    doc.setPageSize(pageSize);
    doc.setDefaultFont(QFont("Sans Serif", 120));

    QString htmlString = convertTemplateToHtml(":/templates/procotol.html");

    QTextCursor cursor(&doc);
    cursor.insertHtml(htmlString);

    QImage image = m_graph.toImage();
    image = image.scaled(1360, 910, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    image.setDevicePixelRatio(0.1);

    QTransform transform;
    //transform.rotate(180);
    image = image.transformed(transform, Qt::TransformationMode::SmoothTransformation);

    cursor.insertImage(image);

    doc.print(&printer);




    emit created();

    Logger::instance().logMessage(Logger::Type::PROTOCOL,
                                  QStringLiteral("Protocol created in %1 seconds").arg(timer.elapsed() / 1000.0));
}
