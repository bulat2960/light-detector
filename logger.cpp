#include "logger.h"

#include <QDateTime>
#include <QDir>
#include <QTextStream>

Logger::Logger()
{
    QString currentPath = QDir::currentPath() + "/logs";
    QDir dir(currentPath);
    if (not dir.exists())
    {
        dir.mkpath(".");
    }


    m_logFile.setFileName(QStringLiteral("logs/log-%1_%2.txt")
                          .arg(QDateTime::currentDateTime().toString("dd-MM-yyyy"))
                          .arg((QDateTime::currentDateTime().toString("hh-mm-ss"))));

    m_dataStream.setDevice(&m_logFile);

    m_types.insert(Type::MODBUS,     "MODBUS:    ");
    m_types.insert(Type::CALCULATOR, "CALCULATOR:");
    m_types.insert(Type::CHART,      "CHART:     ");
    m_types.insert(Type::PROTOCOL,   "PROTOCOL:  ");
    m_types.insert(Type::EXPERIMENT, "EXPERIMENT:");
}

void Logger::setLoggingEnabled(bool enabled)
{
    m_loggingEnabled = enabled;
}

bool Logger::isLoggingEnabled() const
{
    return m_loggingEnabled;
}

Logger& Logger::instance()
{
    static Logger inst;
    return inst;
}

void Logger::logMessage(Type type, const QString &message)
{   
    if (not m_loggingEnabled)
    {
        return;
    }

    if (not m_logFile.isOpen())
    {
        m_logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    }

    QString currentDateTime = QDateTime::currentDateTime().toString("dd-MM-yy hh:mm:ss").toUtf8();
    QString str = QStringLiteral("[%1], %2 %3\n")
                  .arg(currentDateTime)
                  .arg(m_types[type])
                  .arg(message);

    m_dataStream << str;
}

Logger::~Logger()
{
    m_logFile.close();
}
