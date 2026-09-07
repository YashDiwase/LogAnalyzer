#include "ParserFactory.h"
#include "TextLogParser.h"
#include "JsonLogParser.h"
#include <algorithm>

std::unique_ptr<ILogParser>
ParserFactory::CreateParser(const std::string& extension)
{
    std::string ext = extension;

    std::transform(
        ext.begin(),
        ext.end(),
        ext.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(std::tolower(c));
        });

    if (ext == ".txt" || ext == ".log")
    {
        return std::make_unique<TextLogParser>();
    }

    if (ext == ".json")
    {
        return std::make_unique<JsonLogParser>();
    }

    return nullptr;
}
