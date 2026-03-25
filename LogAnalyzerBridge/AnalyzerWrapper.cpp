#include "pch.h"

#pragma managed(push, off)
#include "../CoreEngine/LogParser.h"
#pragma managed(pop)

#include <msclr/marshal_cppstd.h>
#include "AnalyzerWrapper.h"

using namespace msclr::interop;

namespace LogAnalyzerBridge
{
    AnalyzerWrapper::AnalyzerWrapper()
    {
        nativeParser = new LogParser();
    }

    AnalyzerWrapper::~AnalyzerWrapper()
    {
        delete static_cast<LogParser*>(nativeParser);
    }

    bool AnalyzerWrapper::LoadFile(String^ path)
    {
        std::string nativePath = marshal_as<std::string>(path);
        return static_cast<LogParser*>(nativeParser)->LoadFile(nativePath);
    }

    int AnalyzerWrapper::GetInfoCount()
    {
        auto counts = static_cast<LogParser*>(nativeParser)->GetLevelCounts();
        return counts[LogLevel::Info];
    }

    int AnalyzerWrapper::GetWarnCount()
    {
        auto counts = static_cast<LogParser*>(nativeParser)->GetLevelCounts();
        return counts[LogLevel::Warn];
    }

    int AnalyzerWrapper::GetErrorCount()
    {
        auto counts = static_cast<LogParser*>(nativeParser)->GetLevelCounts();
        return counts[LogLevel::Error];
    }

    Dictionary<String^, int>^ AnalyzerWrapper::GetModuleErrorCounts()
    {
        auto nativeMap =
            static_cast<LogParser*>(nativeParser)->GetErrorModuleCounts();

        Dictionary<String^, int>^ managedMap =
            gcnew Dictionary<String^, int>();

        for (auto& pair : nativeMap)
        {
            managedMap->Add(
                gcnew String(pair.first.c_str()),
                pair.second
            );
        }

        return managedMap;
    }
    int AnalyzerWrapper::GetMalformedLineCount()
    {
        return static_cast<LogParser*>(nativeParser)->GetMalformedLineCount();
    }
}