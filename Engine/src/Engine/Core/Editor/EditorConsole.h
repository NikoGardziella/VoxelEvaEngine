#pragma once

#include <mutex>
#include <string>
#include <vector>

namespace Engine
{
    enum class ConsoleLogLevel
    {
        Trace,
        Info,
        Warning,
        Error,
        Critical
    };

    struct ConsoleLogEntry
    {
        ConsoleLogLevel level;
        std::string loggerName;
        std::string message;
    };

    class EditorConsole
    {
    public:
        static void AddLog(ConsoleLogLevel level, const std::string& loggerName, const std::string& message);

        static std::vector<ConsoleLogEntry> GetEntriesCopy();

        static void Clear();

    private:
        static std::mutex s_mutex;
        static std::vector<ConsoleLogEntry> s_entries;
    };
}