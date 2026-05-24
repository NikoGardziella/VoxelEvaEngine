#include "ViewportPanel.h"

#include <imgui.h>

namespace Editor
{
    void ViewportPanel::OnImGuiRender()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("Viewport");

        m_isFocused = ImGui::IsWindowFocused();
        m_isHovered = ImGui::IsWindowHovered();

        ImVec2 viewportPanelMin = ImGui::GetWindowContentRegionMin();
        ImVec2 viewportPanelMax = ImGui::GetWindowContentRegionMax();
        ImVec2 viewportWindowPos = ImGui::GetWindowPos();

        m_boundsMin = { viewportWindowPos.x + viewportPanelMin.x, viewportWindowPos.y + viewportPanelMin.y };
        m_boundsMax = { viewportWindowPos.x + viewportPanelMax.x, viewportWindowPos.y + viewportPanelMax.y };

        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        m_size = { availableSize.x, availableSize.y };

        if (m_viewportTexture)
        {
            ImGui::Image(m_viewportTexture, availableSize, ImVec2(0, 1), ImVec2(1, 0));
        }
        else
        {
            ImGui::Dummy(availableSize);

            ImVec2 textSize = ImGui::CalcTextSize("Viewport texture not connected yet");
            ImVec2 cursorPos = ImGui::GetWindowPos();

            ImGui::SetCursorScreenPos(ImVec2(
                cursorPos.x + availableSize.x * 0.5f - textSize.x * 0.5f,
                cursorPos.y + availableSize.y * 0.5f - textSize.y * 0.5f
            ));

            ImGui::TextDisabled("Viewport texture not connected yet");
        }

        ImGui::End();

        ImGui::PopStyleVar();
    }

    void ViewportPanel::SetViewportTexture(ImTextureID textureId)
    {
        m_viewportTexture = textureId;
    }

    void ViewportPanel::ClearViewportTexture()
    {
        m_viewportTexture = 0;
    }
}