#include "LogParser.h"
#include <fstream>
#include <sstream>

#include <thread>
#include <mutex>
#include <future>

bool LogParser::LoadFile(const std::string& path)
{
    Reset();
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line))
    {
        if (!line.empty())
            lines.push_back(line);
    }

    const unsigned int numThreads = std::thread::hardware_concurrency();
    //const unsigned int numThreads = 1;
    const size_t chunkSize = lines.size() / numThreads + 1;

    std::vector<std::future<void>> futures;

    std::mutex mergeMutex;

    for (unsigned int t = 0; t < numThreads; ++t)
    {
        size_t start = t * chunkSize;
        size_t end = std::min(start + chunkSize, lines.size());

        if (start >= lines.size())
            break;

        futures.push_back(std::async(std::launch::async,
            [this, &lines, start, end, &mergeMutex]()
            {
                std::unordered_map<LogLevel, int> localLevelCounts;
                std::unordered_map<std::string, int> localErrorModuleCounts;

                for (size_t i = start; i < end; ++i)
                {
                    LogEntry entry;

                    if (!ParseLine(lines[i], entry))
                    {
                        malformedLineCount++;
                        continue;
                    }

                    localLevelCounts[entry.level]++;

                    if (entry.level == LogLevel::Error)
                        localErrorModuleCounts[entry.module]++;
                }

                // Merge results safely
                std::lock_guard<std::mutex> lock(mergeMutex);

                for (auto& entry1 : localLevelCounts)
                    levelCounts[entry1.first] += entry1.second;

                for (auto& entry2 : localErrorModuleCounts)
                    errorModuleCounts[entry2.first] += entry2.second;
            }));
    }

    for (auto& f : futures)
        f.get();

    return true;
}


bool LogParser::ParseLine(const std::string& line, LogEntry& entry)
{
    size_t pos1 = line.find(' ');
    if (pos1 == std::string::npos) return false;

    size_t pos2 = line.find(' ', pos1 + 1);
    if (pos2 == std::string::npos) return false;

    size_t pos3 = line.find(' ', pos2 + 1);
    if (pos3 == std::string::npos) return false;

    size_t pos4 = line.find(' ', pos3 + 1);
    if (pos4 == std::string::npos) return false;

    size_t dashPos = line.find(" - ");
    if (dashPos == std::string::npos) return false;

    std::string date = line.substr(0, pos1);
    std::string time = line.substr(pos1 + 1, pos2 - pos1 - 1);
    std::string levelStr = line.substr(pos2 + 1, pos3 - pos2 - 1);
    std::string module = line.substr(pos3 + 1, pos4 - pos3 - 1);

    entry.timestamp = date + " " + time;
    entry.level = ParseLogLevel(levelStr);
    entry.module = module;

    entry.message = line.substr(dashPos + 3); // skip " - "

    return true;
}



LogLevel LogParser::ParseLogLevel(const std::string& levelStr)
{
    if (levelStr == "INFO")  return LogLevel::Info;
    if (levelStr == "WARN")  return LogLevel::Warn;
    if (levelStr == "ERROR") return LogLevel::Error;

    return LogLevel::Unknown;
}


const std::unordered_map<LogLevel, int>&
LogParser::GetLevelCounts() const
{
    return levelCounts;
}

const std::unordered_map<std::string, int>&
LogParser::GetErrorModuleCounts() const
{
    return errorModuleCounts;
}
size_t LogParser::GetMalformedLineCount() const
{
    return malformedLineCount;
}
void LogParser::Reset()
{
    levelCounts.clear();
    errorModuleCounts.clear();
    malformedLineCount = 0;
}