#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>

class Logger {
public:
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    static void setLogLevel(LogLevel level);
    static void setLogFile(const std::string& filename);

    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);

private:
    static LogLevel currentLevel;
    static std::ofstream logFile;

    static void log(LogLevel level, const std::string& message);
    static std::string getTimestamp();
};

#endif // LOGGER_HPP