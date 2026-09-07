#pragma once
#include "LogEntry.h"
#include "ILogParser.h"
#include <vector>
#include <unordered_map>

class TextLogParser : public ILogParser {
public:
    bool LoadFile(const std::string& path);

    const std::vector<LogEntry>&
        GetLogs() const override;

    size_t GetMalformedLineCount() const override;
    
private:
    bool ParseLine(const std::string& line, LogEntry& entry);
    LogLevel ParseLogLevel(const std::string& levelStr);
    void Reset();
    // REMOVE large vector if not needed
     std::vector<LogEntry> logs;
    size_t malformedLineCount = 0;
};
