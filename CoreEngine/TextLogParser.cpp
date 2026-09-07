#include "TextLogParser.h"
#include <fstream>
#include <sstream>

#include <thread>
#include <mutex>
#include <future>
#include <iterator>

bool TextLogParser::LoadFile(const std::string& path)
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

        futures.push_back(
            std::async(
                std::launch::async,
                [this,
                &lines,
                start,
                end,
                &mergeMutex]()
                {
                    std::vector<LogEntry> localLogs;

                    size_t localMalformedCount = 0;

                    for (size_t i = start; i < end; ++i)
                    {
                        LogEntry entry;

                        if (!ParseLine(lines[i], entry))
                        {
                            localMalformedCount++;
                            continue;
                        }

                        localLogs.push_back(
                            std::move(entry));
                    }

                    std::lock_guard<std::mutex> lock(mergeMutex);

                    logs.insert(
                        logs.end(),
                        std::make_move_iterator(
                            localLogs.begin()),
                        std::make_move_iterator(
                            localLogs.end()));

                    malformedLineCount +=
                        localMalformedCount;
                }));
    }

    for (auto& f : futures)
        f.get();

    return true;
}


bool TextLogParser::ParseLine(const std::string& line, LogEntry& entry)
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



LogLevel TextLogParser::ParseLogLevel(const std::string& levelStr)
{
    if (levelStr == "INFO")  return LogLevel::Info;
    if (levelStr == "WARN")  return LogLevel::Warn;
    if (levelStr == "ERROR") return LogLevel::Error;

    return LogLevel::Unknown;
}


size_t TextLogParser::GetMalformedLineCount() const
{
    return malformedLineCount;
}
void TextLogParser::Reset()
{
    logs.clear();
    malformedLineCount = 0;
}
const std::vector<LogEntry>&
TextLogParser::GetLogs() const
{
    return logs;
}