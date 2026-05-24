#pragma once

#include <glm/glm.hpp>
#include <imgui.h>

namespace Editor
{
    class ViewportPanel
    {
    public:
        ViewportPanel() = default;

        void OnImGuiRender();
        void SetViewportTexture(ImTextureID textureId);
        void ClearViewportTexture();

        bool IsFocused() const { return m_isFocused; }
        bool IsHovered() const { return m_isHovered; }

        glm::vec2 GetSize() const { return m_size; }
        glm::vec2 GetBoundsMin() const { return m_boundsMin; }
        glm::vec2 GetBoundsMax() const { return m_boundsMax; }

    private:
        ImTextureID m_viewportTexture = 0;

        bool m_isFocused = false;
        bool m_isHovered = false;

        glm::vec2 m_size = { 0.0f, 0.0f };
        glm::vec2 m_boundsMin = { 0.0f, 0.0f };
        glm::vec2 m_boundsMax = { 0.0f, 0.0f };
    };
}