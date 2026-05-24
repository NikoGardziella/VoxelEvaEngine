#pragma once

#include "EditorConsole.h"

#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/pattern_formatter.h>

#include <mutex>
#include <string>

namespace Engine
{
    template<typename Mutex>
    class EditorConsoleSink : public spdlog::sinks::base_sink<Mutex>
    {
    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            spdlog::memory_buf_t formatted;
            this->formatter_->format(msg, formatted);

            ConsoleLogLevel level = ConsoleLogLevel::Info;

            switch (msg.level)
            {
            case spdlog::level::trace:
                level = ConsoleLogLevel::Trace;
                break;

            case spdlog::level::info:
                level = ConsoleLogLevel::Info;
                break;

            case spdlog::level::warn:
                level = ConsoleLogLevel::Warning;
                break;

            case spdlog::level::err:
                level = ConsoleLogLevel::Error;
                break;

            case spdlog::level::critical:
                level = ConsoleLogLevel::Critical;
                break;

            default:
                level = ConsoleLogLevel::Info;
                break;
            }

            std::string loggerName(msg.logger_name.data(), msg.logger_name.size());
            std::string text = fmt::to_string(formatted);

            EditorConsole::AddLog(level, loggerName, text);
        }

        void flush_() override
        {
        }
    };

    using EditorConsoleSink_mt = EditorConsoleSink<std::mutex>;
}