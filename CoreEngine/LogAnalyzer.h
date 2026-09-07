#pragma once

#include "LogEntry.h"

#include <string>
#include <unordered_map>
#include <vector>

class LogAnalyzer
{
public:
    void Analyze(const std::vector<LogEntry>& logs);

    const std::unordered_map<LogLevel, int>&
        GetLevelCounts() const;

    const std::unordered_map<std::string, int>&
        GetErrorModuleCounts() const;

private:
    void Reset();

private:
    std::unordered_map<LogLevel, int> levelCounts;

    std::unordered_map<std::string, int>
        errorModuleCounts;
};