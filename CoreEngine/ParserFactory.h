#pragma once

#include <memory>
#include <string>

#include "ILogParser.h"

class ParserFactory
{
public:
    static std::unique_ptr<ILogParser>
        CreateParser(const std::string& extension);
};