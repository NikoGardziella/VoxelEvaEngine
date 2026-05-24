#include "EditorConsole.h"

namespace Engine
{
    std::mutex EditorConsole::s_mutex;
    std::vector<ConsoleLogEntry> EditorConsole::s_entries;

    void EditorConsole::AddLog(ConsoleLogLevel level, const std::string& loggerName, const std::string& message)
    {
        std::scoped_lock lock(s_mutex);

        s_entries.push_back({
            level,
            loggerName,
            message
            });

        constexpr size_t maxEntries = 1000;

        if (s_entries.size() > maxEntries)
        {
            s_entries.erase(s_entries.begin());
        }
    }

    std::vector<ConsoleLogEntry> EditorConsole::GetEntriesCopy()
    {
        std::scoped_lock lock(s_mutex);
        return s_entries;
    }

    void EditorConsole::Clear()
    {
        std::scoped_lock lock(s_mutex);
        s_entries.clear();
    }
}