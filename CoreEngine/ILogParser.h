#pragma once

#include "LogEntry.h"
#include <string>
#include <vector>

class ILogParser
{
public:

    virtual ~ILogParser() = default;

    virtual bool LoadFile(const std::string& path) = 0;

    virtual const std::vector<LogEntry>& GetLogs() const = 0;

    virtual size_t GetMalformedLineCount() const = 0;
};