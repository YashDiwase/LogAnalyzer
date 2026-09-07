#pragma once

#include "ILogParser.h"

class JsonLogParser: public ILogParser
{
public:

    bool LoadFile(const std::string& path);

    const std::vector<LogEntry>& GetLogs() const override;

    size_t GetMalformedLineCount() const override;

private:

    std::vector<LogEntry> logs;

    size_t malformedRecordCount = 0;
};