#pragma once

using namespace System;
using namespace System::Collections::Generic;

namespace LogAnalyzerBridge
{
    public ref class AnalyzerWrapper
    {
    private:
        void* nativeParser;

    public:
        AnalyzerWrapper();
        ~AnalyzerWrapper();

        bool LoadFile(String^ path);

        int GetInfoCount();
        int GetWarnCount();
        int GetErrorCount();
        int GetMalformedLineCount();
        Dictionary<String^, int>^ GetModuleErrorCounts();
    };
}