#pragma once

#include "Engine/Core/Layer.h"
#include <imgui.h>
#include <Editor/Panels/ConsolePanel.h>
#include <Editor/Panels/ViewportPanel.h>
#include <Engine/Renderer/Camera/EditorCamera.h>
#include <Editor/Panels/AssetBrowserPanel.h>

namespace Engine
{
    class VulkanRenderer;
}

namespace Editor {

   

    class EditorLayer : public Engine::Layer
    {
    public:
        EditorLayer(Engine::VulkanRenderer* renderer);
        ~EditorLayer() override;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(Engine::Timestep ts) override;
        void OnImGuiRender() override;
        void DrawSceneHierarchyPanel();
        void DrawInspectorPanel();
        void OnEvent(Engine::Event& event) override;

        Editor::ViewportPanel& GetViewportPanel() { return m_viewportPanel; }

    private: 

        Engine::EditorCamera m_editorCamera;
        
        ConsolePanel m_consolePanel;
        ViewportPanel m_viewportPanel;
        AssetBrowserPanel m_assetBrowserPanel;
        Engine::VulkanRenderer* m_renderer = nullptr;

        ImVec2 m_viewportBoundsMin{};
        ImVec2 m_viewportBoundsMax{};
        bool m_viewportFocused = false;
        bool m_viewportHovered = false;

    };


}
