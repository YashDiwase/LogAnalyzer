#pragma once
#include <string>

enum class LogLevel {
    Info,
    Warn,
    Error,
    Unknown
};

struct LogEntry {
    std::string timestamp;
    LogLevel level;
    std::string module;
    std::string message;
};
