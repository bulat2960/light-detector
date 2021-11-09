#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QMap>
#include <QTextStream>

class Logger
{
public:
    static Logger& instance();
    ~Logger();

    enum class Type
    {
        MODBUS,
        CALCULATOR,
        CHART,
        PROTOCOL,
        EXPERIMENT
    };

    void setLoggingEnabled(bool enabled);
    bool isLoggingEnabled() const;

    void logMessage(Type type, const QString& message);

private:
    Logger();
    Logger(const Logger& logger) = delete;
    Logger& operator=(const Logger& logger) = delete;

    QFile m_logFile;
    QTextStream m_dataStream;

    QMap<Type, QString> m_types;

    bool m_loggingEnabled {false};
};

#endif // LOGGER_H
