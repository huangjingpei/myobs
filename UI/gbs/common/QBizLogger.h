#ifndef __Q_BIZ_LOGGER_H__
#define __Q_BIZ_LOGGER_H__
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QString>
#include <windows.h>
#include <QMutex>
#include <QThread>
#include <QQueue>
#include <QWaitCondition>
#include <QVarLengthArray>

namespace QLog {
enum LogLevel {
    QDEBUG,
    QINFO,
    QWARNING,
    QERROR
};

class LoggerThread : public QThread {
    Q_OBJECT

public:
    void run() override {
        while (true) {
            QString message;
            {
                QMutexLocker locker(&mutex);
                if (queue.isEmpty()) {
                    waitCondition.wait(&mutex);
                }
                if (!queue.isEmpty()) {
                    message = queue.dequeue();
                }
            }
            if (!message.isEmpty()) {
                // 写入文件
                if (logFile.isOpen()) {
                    QTextStream out(&logFile);
                    out << message << "\n";
                    out.flush();
                }

                //OutputDebugStringA(message.toLocal8Bit().constData());

            }
        }
    }

    void enqueue(const QString &message) {
        {
            QMutexLocker locker(&mutex);
            queue.enqueue(message);
        }
        waitCondition.wakeOne(); // 唤醒日志线程
    }

    void setLogFile(const QString &fileName) {
        logFile.setFileName(fileName);
        logFile.open(QIODevice::Append | QIODevice::Text);
    }

private:
    QFile logFile;
    QQueue<QString> queue; // 缓冲队列
    QMutex mutex; // 互斥锁
    QWaitCondition waitCondition; // 等待条件
};

class Logger {
public:
    static Logger& instance() {
        static Logger instance;
        return instance;
    }

    void configure(const QString &fileName, LogLevel level = QINFO) {
        loggerThread.setLogFile(fileName);
        loggerThread.start();
        logLevel = level;
    }
    void log(LogLevel level, const QString &file, const QString &function, int line, const QString format, ... ) {
        if (level >= logLevel) return; // 过滤低于当前日志级别的日志
        va_list list;
        va_start(list, format);
        QString message = QString(format).vasprintf(format.toStdString().c_str(), list);
        va_end(list);
        log2(level, file, function, line, message);
    }
private:
    void log2(LogLevel level, const QString &file, const QString &function, int line, const QString &message) {
        if (level >= logLevel) return; // 过滤低于当前日志级别的日志

        QString logMessage = QString("[%1] [%2] %3")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"),
                                      logLevelToString(level), message);

        // 将日志消息放入队列
        loggerThread.enqueue(logMessage);

        // 输出到 Debug Viewer
        OutputDebugStringA(logMessage.toLocal8Bit().constData());
    }

private:
    Logger() = default; // 禁止外部实例化
    ~Logger() {
        loggerThread.quit();
        loggerThread.wait();
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    QString logLevelToString(LogLevel level) const {
        switch (level) {
        case QDEBUG:   return "DEBUG";
        case QINFO:    return "INFO";
        case QWARNING: return "WARNING";
        case QERROR:   return "ERROR";
        default:      return "UNKNOWN";
        }
    }

    LoggerThread loggerThread; // 日志线程

    LogLevel logLevel = QINFO;
};

} //namespace QLog

#define QLogD(...)                                 \
(QLog::Logger::instance().log(QLog::LogLevel::QDEBUG, __FILE__, __FUNCTION__, __LINE__, \
                            __VA_ARGS__))
#define QLogI(...)                                 \
(QLog::Logger::instance().log(QLog::LogLevel::QINFO, __FILE__, __FUNCTION__,__LINE__, \
                               __VA_ARGS__))
#define QLogW(...)                                 \
(QLog::Logger::instance().log(QLog::LogLevel::QWARNING, __FILE__, __FUNCTION__,__LINE__, \
                               __VA_ARGS__))
#define QLogE(...)                                 \
(QLog::Logger::instance().log(QLog::LogLevel::QERROR, __FILE__, __FUNCTION__,__LINE__, \
                               __VA_ARGS__))
#endif //__Q_BIZ_LOGGER_H__
