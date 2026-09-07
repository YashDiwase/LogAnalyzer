#include "LogAnalyzer.h"

void LogAnalyzer::Analyze(
    const std::vector<LogEntry>& logs)
{
    Reset();

    for (const auto& entry : logs)
    {
        levelCounts[entry.level]++;

        if (entry.level == LogLevel::Error)
        {
            errorModuleCounts[entry.module]++;
        }
    }
}

const std::unordered_map<LogLevel, int>&
LogAnalyzer::GetLevelCounts() const
{
    return levelCounts;
}

const std::unordered_map<std::string, int>&
LogAnalyzer::GetErrorModuleCounts() const
{
    return errorModuleCounts;
}

void LogAnalyzer::Reset()
{
    levelCounts.clear();
    errorModuleCounts.clear();
}