#pragma once

#include <string>
#include <iostream>
#include <mutex>

using std::mutex;
using std::string;

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

// Simple thread-safe logger
class Logger {
public:
    static void setLogLevel(LogLevel level) {
        getLogLevel() = level;
    }

    static void log(LogLevel level, const string& message) {
        if (level < getLogLevel()) return; // Skip logging if below current level
        std::lock_guard<std::mutex> lock(getMutex());
        std::ostream& out = (level == LogLevel::Error) ? std::cerr : std::cout;
        out << "[" << levelToString(level) << "] " << message << std::endl;
    }

    static void debug(const string& message)    { log(LogLevel::Debug, message); }
    static void info(const string& message)     { log(LogLevel::Info, message); }
    static void warning(const string& message)  { log(LogLevel::Warning, message); }
    static void error(const string& message)    { log(LogLevel::Error, message); }

private:
    static string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Error:   return "ERROR";
            default:                return "LOG";
        }
    }

    static mutex& getMutex() {
        static mutex mtx;
        return mtx;
    }

    static LogLevel& getLogLevel() {
        static LogLevel currentLevel = LogLevel::Error; // Default Error and above
        return currentLevel;
    }


};