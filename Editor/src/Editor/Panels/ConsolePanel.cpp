#include "ConsolePanel.h"


#include <imgui.h>
#include <vector>
#include <Engine/Core/Editor/EditorConsole.h>

namespace Editor
{
    void ConsolePanel::OnImGuiRender()
    {
        ImGui::Begin("Console");

        if (ImGui::Button("Clear"))
        {
            Engine::EditorConsole::Clear();
        }

        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &m_autoScroll);

        ImGui::Separator();

        ImGui::BeginChild("ConsoleScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        std::vector<Engine::ConsoleLogEntry> entries = Engine::EditorConsole::GetEntriesCopy();

        for (const Engine::ConsoleLogEntry& entry : entries)
        {
            ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

            switch (entry.level)
            {
            case Engine::ConsoleLogLevel::Trace:
                color = ImVec4(0.65f, 0.65f, 0.65f, 1.0f);
                break;

            case Engine::ConsoleLogLevel::Info:
                color = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
                break;

            case Engine::ConsoleLogLevel::Warning:
                color = ImVec4(1.0f, 0.8f, 0.2f, 1.0f);
                break;

            case Engine::ConsoleLogLevel::Error:
                color = ImVec4(1.0f, 0.25f, 0.25f, 1.0f);
                break;

            case Engine::ConsoleLogLevel::Critical:
                color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                break;
            }

            ImGui::TextColored(color, "%s", entry.message.c_str());
        }

        if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();

        ImGui::End();
    }
}