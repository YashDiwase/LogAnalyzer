#include "JsonLogParser.h"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool JsonLogParser::LoadFile(const std::string& path)
{
    logs.clear();
    malformedRecordCount = 0;

    std::ifstream file(path);

    if (!file.is_open())
    {
        return false;
    }

    try
    {
        json data;
        file >> data;

        if (!data.is_array())
        {
            return false;
        }

        for (const auto& item : data)
        {
            try
            {
                LogEntry entry;

                entry.timestamp = item.at("timestamp").get<std::string>();
                entry.module = item.at("module").get<std::string>();
                entry.message = item.at("message").get<std::string>();

                std::string level =
                    item.at("level").get<std::string>();

                if (level == "INFO")
                {
                    entry.level = LogLevel::Info;
                }
                else if (level == "WARN")
                {
                    entry.level = LogLevel::Warn;
                }
                else if (level == "ERROR")
                {
                    entry.level = LogLevel::Error;
                }
                else
                {
                    entry.level = LogLevel::Unknown;
                }

                logs.push_back(std::move(entry));
            }
            catch (const json::exception&)
            {
                malformedRecordCount++;
            }
        }
    }
    catch (const json::exception&)
    {
        return false;
    }

    return true;
}

const std::vector<LogEntry>& JsonLogParser::GetLogs() const
{
    return logs;
}

size_t JsonLogParser::GetMalformedLineCount() const
{
    return malformedRecordCount;
}