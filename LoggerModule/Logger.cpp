#include "Logger.h"

using namespace LoggerModule;

Logger::Logger(const LoggerConfig& config) : config(config) {
    if (!config.filename.empty()) {
        logFile.open(config.filename, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Error: Could not open log file: " << config.filename << std::endl;
        }
    }

    if (config.flushInterval > 0) {
        flushThread = std::thread(&Logger::flushThreadFunc, this);
    }
}

Logger::~Logger() {
    if (flushThread.joinable()) {
        flushThread.join();
    }
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < config.level) {
        return;
    }

    std::string timestamp = getCurrentTimestamp();
    std::string logLine = timestamp + " [" + getLevelName(level) + "] " + message + "\n";

    std::cout << logLine;

    if (logFile.is_open()) {
        std::lock_guard<std::mutex> lock(logMutex);
        logFile << logLine;
    }

    if (config.flushInterval > 0) {
        autoFlush();
    }
}

Logger::Logger(Logger&& other) noexcept
    : config(std::move(other.config)),
    logFile(std::move(other.logFile)),
    flushThread(std::move(other.flushThread)) {}

Logger& Logger::operator=(Logger&& other) noexcept {
    if (this != &other) {
        {
            std::lock_guard<std::mutex> lock(logMutex);
            std::lock_guard<std::mutex> flushLock(flushMutex);

            config = std::move(other.config);
            logFile = std::move(other.logFile);
        }

        if (flushThread.joinable()) {
            flushThread.join();
        }
        flushThread = std::move(other.flushThread);
    }
    return *this;
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &time_t_now);
#else
    localtime_r(&time_t_now, &tm);
#endif
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Logger::getLevelName(LogLevel level) {
    switch (level) {
    case TRACE: return "TRACE";
    case DEBUG: return "DEBUG";
    case INFO:  return "INFO";
    case WARN:  return "WARN";
    case ERROR: return "ERROR";
    case FATAL: return "FATAL";
    default: return "UNKNOWN";
    }
}

void Logger::autoFlush() {
    std::unique_lock<std::mutex> lock(flushMutex);
    flushCondVar.wait_for(lock, std::chrono::milliseconds(config.flushInterval));
}

void Logger::flushThreadFunc() {
    while (true) {
        std::unique_lock<std::mutex> lock(flushMutex);
        flushCondVar.wait_for(lock, std::chrono::milliseconds(config.flushInterval));

        if (logFile.is_open()) {
            logFile.flush();
        }
    }
}