#include "Log.h"



#include <spdlog/sinks/stdout_color_sinks.h>
#include "Editor/EditorConsoleSink.h"


namespace Engine
{
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    void Log::Init()
    {
        std::vector<spdlog::sink_ptr> coreSinks;
        std::vector<spdlog::sink_ptr> clientSinks;

        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_pattern("%^[%T] [%n] %v%$");

        auto editorSink = std::make_shared<EditorConsoleSink_mt>();
        editorSink->set_pattern("[%T] [%n] [%l] %v");

        coreSinks.push_back(consoleSink);
        coreSinks.push_back(editorSink);

        clientSinks.push_back(consoleSink);
        clientSinks.push_back(editorSink);

        s_CoreLogger = std::make_shared<spdlog::logger>("Engine", coreSinks.begin(), coreSinks.end());
        s_ClientLogger = std::make_shared<spdlog::logger>("App", clientSinks.begin(), clientSinks.end());

        s_CoreLogger->set_level(spdlog::level::trace);
        s_ClientLogger->set_level(spdlog::level::trace);

        s_CoreLogger->flush_on(spdlog::level::trace);
        s_ClientLogger->flush_on(spdlog::level::trace);

        spdlog::register_logger(s_CoreLogger);
        spdlog::register_logger(s_ClientLogger);
    }
}