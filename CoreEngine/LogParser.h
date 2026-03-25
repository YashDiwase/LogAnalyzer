#pragma once
#include "LogEntry.h"
#include <vector>
#include <unordered_map>

class LogParser {
public:
    bool LoadFile(const std::string& path);

    // Accessors
    const std::unordered_map<LogLevel, int>& GetLevelCounts() const;
    const std::unordered_map<std::string, int>& GetErrorModuleCounts() const;
    size_t GetMalformedLineCount() const;
    
private:
    bool ParseLine(const std::string& line, LogEntry& entry);
    LogLevel ParseLogLevel(const std::string& levelStr);
    void Reset();
    // REMOVE large vector if not needed
    // std::vector<LogEntry> logs;
    size_t malformedLineCount = 0;
    std::unordered_map<LogLevel, int> levelCounts;
    std::unordered_map<std::string, int> errorModuleCounts;
};
