#pragma once

#include "Engine/Core/Layer.h"
#include <imgui.h>
#include <Editor/Panels/ConsolePanel.h>
#include <Editor/Panels/ViewportPanel.h>
#include <Engine/Renderer/Camera/EditorCamera.h>
#include <Editor/Panels/AssetBrowserPanel.h>
#include <Engine/Scene/Scene.h>
#include <Editor/Tools/AssetPlacementTool.h>
#include <Engine/Scene/Entity.h>
#include <Editor/EditorState.h>
#include <Engine/Scene/SceneTypes.h>
#include <Editor/Panels/SceneHierarchyPanel.h>
#include <Editor/Panels/InspectorPanel.h>
#include "Editor/EditorSelection.h"



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
        void OnRender() override;
        void OnScenePlay();
        void OnSceneStop();
        Engine::Scene& GetActiveScene();
        void DrawSceneHierarchyPanel();
        void DrawInspectorPanel();
        void OnEvent(Engine::Event& event) override;

        Editor::ViewportPanel& GetViewportPanel() { return m_viewportPanel; }

    private: 

        Engine::EditorCamera m_editorCamera;
        
        ConsolePanel m_consolePanel;
        ViewportPanel m_viewportPanel;
        AssetBrowserPanel m_assetBrowserPanel;
        SceneHierarchyPanel m_sceneHierarchyPanel;
        InspectorPanel m_inspectorPanel;

        Engine::Scene m_editorScene;
        std::unique_ptr<Engine::Scene> m_runtimeScene;

        SceneState m_sceneState = SceneState::Edit;

        Editor::AssetPlacementTool m_assetPlacementTool;

        Engine::VulkanRenderer* m_renderer = nullptr;

        Engine::SimulationTick m_simulationTick = 0;

        glm::vec2 m_viewportBoundsMin{};
        glm::vec2 m_viewportBoundsMax{};
        bool m_viewportFocused = false;
        bool m_viewportHovered = false;


        EditorSelection m_selection;

    };


}
