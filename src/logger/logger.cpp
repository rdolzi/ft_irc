#include "logger.hpp"
#include <iostream>
#include <ctime>
#include <iomanip>

Logger::LogLevel Logger::currentLevel = Logger::INFO;
std::ofstream Logger::logFile;

void Logger::setLogLevel(LogLevel level) {
    currentLevel = level;
}

void Logger::setLogFile(const std::string& filename) {
    if (logFile.is_open()) {
        logFile.close();
    }
    logFile.open(filename.c_str(), std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

void Logger::debug(const std::string& message) {
    log(DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(INFO, message);
}

void Logger::warning(const std::string& message) {
    log(WARNING, message);
}

void Logger::error(const std::string& message) {
    log(ERROR, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < currentLevel) return;

    std::string levelStr;
    switch (level) {
        case DEBUG: levelStr = "DEBUG"; break;
        case INFO: levelStr = "INFO"; break;
        case WARNING: levelStr = "WARNING"; break;
        case ERROR: levelStr = "ERROR"; break;
    }

    std::string logMessage = getTimestamp() + " [" + levelStr + "] " + message;

    std::cout << logMessage << std::endl;
    if (logFile.is_open()) {
        logFile << logMessage << std::endl;
    }
}

std::string Logger::getTimestamp() {
    std::time_t now = std::time(0);
    std::tm* localTime = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    return std::string(buffer);
}