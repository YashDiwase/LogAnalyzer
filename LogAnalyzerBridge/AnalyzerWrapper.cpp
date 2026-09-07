#include "pch.h"

#pragma managed(push, off)

#include "../CoreEngine/ILogParser.h"
#include "../CoreEngine/ParserFactory.h"
#include "../CoreEngine/LogAnalyzer.h"

#pragma managed(pop)

#include <msclr/marshal_cppstd.h>
#include "AnalyzerWrapper.h"
#include <filesystem>
using namespace msclr::interop;

namespace LogAnalyzerBridge
{
    AnalyzerWrapper::AnalyzerWrapper()
    {
        parser = nullptr;
        analyzer = new LogAnalyzer();
    }

   
    AnalyzerWrapper::~AnalyzerWrapper()
    {
        delete static_cast<ILogParser*>(parser);
        parser = nullptr;

        delete static_cast<LogAnalyzer*>(analyzer);
        analyzer = nullptr;
    }
    

    bool AnalyzerWrapper::LoadFile(String^ path)
    {
        std::string nativePath =
            marshal_as<std::string>(path);

        // Remove previous parser
        delete static_cast<ILogParser*>(parser);
        parser = nullptr;

        // Determine file extension
        std::filesystem::path filePath(nativePath);

        std::string extension =
            filePath.extension().string();

        // Ask factory for appropriate parser
        std::unique_ptr<ILogParser> newParser =
            ParserFactory::CreateParser(extension);

        if (!newParser)
        {
            return false;
        }

        // Load file
        if (!newParser->LoadFile(nativePath))
        {
            return false;
        }

        // Transfer ownership to Bridge
        parser = newParser.release();

        // Analyze parsed logs
        auto nativeParser =
            static_cast<ILogParser*>(parser);

        auto nativeAnalyzer =
            static_cast<LogAnalyzer*>(analyzer);

        nativeAnalyzer->Analyze(
            nativeParser->GetLogs());

        return true;
    }

    int AnalyzerWrapper::GetInfoCount()
    {
        auto nativeAnalyzer =
            static_cast<LogAnalyzer*>(analyzer);

        const auto& counts =
            nativeAnalyzer->GetLevelCounts();

        auto it = counts.find(LogLevel::Info);

        if (it == counts.end())
            return 0;

        return it->second;
    }

    int AnalyzerWrapper::GetWarnCount()
    {
        auto nativeAnalyzer =
            static_cast<LogAnalyzer*>(analyzer);

        const auto& counts =
            nativeAnalyzer->GetLevelCounts();

        auto it = counts.find(LogLevel::Warn);

        if (it == counts.end())
            return 0;

        return it->second;
    }

    int AnalyzerWrapper::GetErrorCount()
    {
        auto nativeAnalyzer =
            static_cast<LogAnalyzer*>(analyzer);

        const auto& counts =
            nativeAnalyzer->GetLevelCounts();

        auto it = counts.find(LogLevel::Error);

        if (it == counts.end())
            return 0;

        return it->second;
    }

    Dictionary<String^, int>^
        AnalyzerWrapper::GetModuleErrorCounts()
    {
        auto nativeAnalyzer =
            static_cast<LogAnalyzer*>(analyzer);

        const auto& counts =
            nativeAnalyzer->GetErrorModuleCounts();

        auto result =
            gcnew Dictionary<String^, int>();

        for (const auto& entry : counts)
        {
            result[
                gcnew String(entry.first.c_str())
            ] = entry.second;
        }

        return result;
    }

    int AnalyzerWrapper::GetMalformedLineCount()
    {
        auto nativeParser =
            static_cast<ILogParser*>(parser);

        if (!nativeParser)
            return 0;

        return static_cast<int>(
            nativeParser->GetMalformedLineCount());
    }
}