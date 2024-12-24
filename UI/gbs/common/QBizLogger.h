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
    QINFO,
    QDEBUG,
    QWARNING,
    QERROR
};

class LoggerThread : public QThread {
    Q_OBJECT

public:
    LoggerThread() : m_exit(false) {}

    void run() override {
        while (!m_exit) {
            QString message;
            {
                QMutexLocker locker(&mutex);
                if (queue.isEmpty() && !m_exit) {
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

    void stop() {
        {
            QMutexLocker locker(&mutex);
            m_exit = true;
        }
        waitCondition.wakeOne();
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
        logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    }

private:
    bool m_exit;
    QFile logFile;
    QQueue<QString> queue;
    QMutex mutex;
    QWaitCondition waitCondition;
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
        if (level < logLevel) return;
        va_list list;
        va_start(list, format);
        QString message = QString(format).vasprintf(format.toStdString().c_str(), list);
        va_end(list);
        log2(level, file, function, line, message);
    }

    // 添加显式退出方法
    void shutdown() {
        loggerThread.stop();
        loggerThread.quit();
        loggerThread.wait();
    }

private:
    void log2(LogLevel level, const QString &file, const QString &function, int line, const QString &message) {
        if (level < logLevel) return;

        QString logMessage = QString("[%1] [%2] %3")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"),
                                      logLevelToString(level), message);

        loggerThread.enqueue(logMessage);

        OutputDebugStringA(logMessage.toLocal8Bit().constData());
    }

private:
    Logger() = default;
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
