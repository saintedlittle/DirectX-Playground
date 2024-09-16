#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <ctime>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <iomanip>
#include <fstream>

namespace LoggerModule {

    #define LOG_MESSAGE(LEVEL, MESSAGE, LOGGER) LOGGER.log(LEVEL, MESSAGE); logger.~Logger();

    enum LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    struct LoggerConfig {
        LogLevel level;
        std::string filename;
        size_t maxFileSize;
        unsigned char maxBackupFiles;
        long flushInterval;
    };

    class Logger {
    public:
        Logger(const LoggerConfig& config);
        ~Logger();
        void log(LogLevel level, const std::string& message);

        // Запрещаем копирование
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        // Разрешаем перемещение
        Logger(Logger&& other) noexcept;
        Logger& operator=(Logger&& other) noexcept;

    private:
        std::string getCurrentTimestamp();
        std::string getLevelName(LogLevel level);
        void autoFlush();
        void flushThreadFunc();

        LoggerConfig config;
        std::ofstream logFile;
        std::mutex logMutex;
        std::mutex flushMutex;
        std::condition_variable flushCondVar;
        std::thread flushThread;
    };

}
#endif // LOGGER_H