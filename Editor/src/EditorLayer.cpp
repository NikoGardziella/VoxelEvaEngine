#include "EditorLayer.h"

#include "Engine/Core/Events/KeyEvent.h"
#include "Engine/Core/Events/MouseEvent.h"
#include "Engine/Core/Log.h"
#include <Engine/Core/Input.h>
#include <GLFW/glfw3.h>
#include <glm/ext/vector_float3.hpp>


#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/MouseCodes.h"
#include "Engine/Core/EntryPoint.h"
#include <Engine/Renderer/GraphicsContext.h>
#include <Engine/Platform/Vulkan/VulkanContext.h>
#include "Engine/Core/Window.h"
#include <imgui_impl_vulkan.h>
#include <Engine/Core/Application.h>
#include "Engine/Platform/Vulkan/VulkanRenderer.h"
#include <Engine/Core/Editor/EditorConsole.h>
#include "Engine/Core/Input.h"
#include "Engine/Core/MouseCodes.h"
#include <Engine/Scene/SceneSerializer/SceneSerializer.h>
#include <Engine/Platform/Windows/FileDialogs.h>


namespace Editor {



    EditorLayer::EditorLayer(Engine::VulkanRenderer* renderer) : Engine::Layer("EditorLayer"), m_renderer(renderer)
    {
        m_assetPlacementTool.SetContext(&m_editorScene, &m_renderer->GetAssetManager());


        m_assetBrowserPanel.SetAssetSelectedCallback([this](const std::filesystem::path& path)
            {
                glm::vec3 spawnPosition = m_editorCamera.GetPosition() + m_editorCamera.GetForwardDirection() * 5.0f;

                Engine::Entity entity = m_assetPlacementTool.CreateEntityFromAsset(path, spawnPosition);
                m_selection.SelectedEntity = entity;

               

                if (m_renderer)
                {
                    m_renderer->LoadVoxelAsset(path);
                }
            });

    }

    EditorLayer::~EditorLayer()
    {
    }

    void EditorLayer::OnAttach()
    {
        m_sceneHierarchyPanel.SetScene(&m_editorScene);
        

        m_sceneHierarchyPanel.SetSelection(&m_selection);
        m_inspectorPanel.SetSelection(&m_selection);

        APP_INFO("EditorLayer attached");
    }

    void EditorLayer::OnDetach()
    {
        APP_INFO("EditorLayer detached");
    }

    void EditorLayer::OnUpdate(Engine::Timestep ts)
    {
        (void)ts;
        m_editorCamera.OnUpdate(ts, m_viewportHovered);

      



        m_editorCamera.OnUpdate(ts, m_viewportPanel.IsHovered());
        m_renderer->GetGameRenderer().SetViewProjection(m_editorCamera.GetViewProjection());



        if (m_sceneState == SceneState::Play)
        {
            Engine::SceneTickContext context{};
            context.Tick = m_simulationTick++;
            context.FixedDeltaTime = 1.0f / 60.0f;

            m_runtimeScene->OnFixedUpdate(context);
        }

     
        if (m_sceneState == SceneState::Play)
        {
            m_runtimeScene->OnRuntimeUpdate(ts);
        }
       
    }




    void EditorLayer::OnImGuiRender()
    {
        m_viewportHovered = GetViewportPanel().IsHovered();
        m_viewportBoundsMin = GetViewportPanel().GetBoundsMin();
        m_viewportBoundsMax = GetViewportPanel().GetBoundsMax();

        static bool dockspaceOpen = true;

        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoDocking;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        windowFlags |=
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGui::Begin("DockSpace", &dockspaceOpen, windowFlags);

        ImGui::PopStyleVar(2);

        ImGuiID dockspaceId = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(
            dockspaceId,
            ImVec2(0.0f, 0.0f),
            ImGuiDockNodeFlags_None
        );

        


        ImGui::End();

        ImTextureID textureId = m_renderer ? m_renderer->GetViewportTextureID() : 0;

        if (textureId != 0)
        {
            m_viewportPanel.SetViewportTexture(textureId);
        }
        else
        {
            m_viewportPanel.ClearViewportTexture();
        }

        m_viewportPanel.OnImGuiRender();
        m_consolePanel.OnImGuiRender();
        m_assetBrowserPanel.OnImGuiRender();
        m_sceneHierarchyPanel.OnImGuiRender();
        m_inspectorPanel.OnImGuiRender();
       // DrawViewportPanel();
        DrawInspectorPanel();
        //DrawConsolePanel();
        m_viewportPanel.OnImGuiRender();

        glm::vec2 viewportSize = m_viewportPanel.GetSize();

        if (viewportSize.x > 0.0f && viewportSize.y > 0.0f)
        {
            m_renderer->RequestViewportResize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
            m_editorCamera.OnResize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
        }

        if (m_viewportHovered && Engine::Input::IsMouseButtonPressed(Engine::Mouse::Button0))
        {
            ImVec2 mouse = ImGui::GetMousePos();

            float viewportWidth = m_viewportBoundsMax.x - m_viewportBoundsMin.x;
            float viewportHeight = m_viewportBoundsMax.y - m_viewportBoundsMin.y;

            float localX = mouse.x - m_viewportBoundsMin.x;
            float localY = mouse.y - m_viewportBoundsMin.y;

            if (localX < 0.0f || localY < 0.0f || localX >= viewportWidth || localY >= viewportHeight)
                return;

            VkExtent2D pickExtent = m_renderer->GetPickingRenderTarget().GetExtent();

            uint32_t pickX = static_cast<uint32_t>((localX / viewportWidth) * pickExtent.width);
            uint32_t pickY = static_cast<uint32_t>((localY / viewportHeight) * pickExtent.height);

            pickY = pickExtent.height - 1 - static_cast<uint32_t>((localY / viewportHeight) * pickExtent.height);
            Engine::ViewportPickResult pick = m_renderer->ReadViewportPick(pickX, pickY);


            if (pick.Hit)
            {
                ENGINE_INFO(
                    "Picked voxel: chunk={}, local=({}, {}, {})",
                    pick.ChunkIndex,
                    pick.LocalVoxel.x,
                    pick.LocalVoxel.y,
                    pick.LocalVoxel.z
                );
            }
        }
        DrawToolbar();
        DrawMenuBar();
    }

    void EditorLayer::OnRender()
    {

        if (m_renderer)
        {
            m_renderer->SetViewProjection(m_editorCamera.GetViewProjection());
            m_renderer->RenderScene(GetActiveScene());

            
            m_renderer->RenderScenePicking(m_editorScene);
            
        }
    }
    
    void EditorLayer::OnScenePlay()
    {
        m_sceneState = SceneState::Play;
        m_runtimeScene = m_editorScene.Copy();
        m_runtimeScene = m_editorScene.Copy();
        m_runtimeScene->OnRuntimeStart();
        m_sceneState = SceneState::Play;
    }

    void EditorLayer::OnSceneStop()
    {
        if(m_runtimeScene)
            m_runtimeScene->OnRuntimeStop();

        m_runtimeScene.reset();
        m_sceneState = SceneState::Edit;
    }

    Engine::Scene& EditorLayer::GetActiveScene()
    {
        if (m_sceneState == SceneState::Edit)
            return m_editorScene;

        return *m_runtimeScene;
    }

    void EditorLayer::DrawToolbar()
    {
        ImGui::Begin("Toolbar");

        if (m_sceneState == SceneState::Edit)
        {
            if (ImGui::Button("Play"))
                OnScenePlay();
        }
        else
        {
            if (ImGui::Button("Stop"))
                OnSceneStop();

            ImGui::SameLine();

            if (m_sceneState == SceneState::Play)
            {
                if (ImGui::Button("Pause"))
                    m_sceneState = SceneState::Pause;
            }
            else if (m_sceneState == SceneState::Pause)
            {
                if (ImGui::Button("Resume"))
                    m_sceneState = SceneState::Play;
            }
        }

        ImGui::End();
    }

    void EditorLayer::DrawInspectorPanel()
    {
        ImGui::Begin("Inspector");

        ImGui::Text("Selected Entity");
        ImGui::Separator();

        float position[3] = { 0.0f, 0.0f, 0.0f };
        float rotation[3] = { 0.0f, 0.0f, 0.0f };
        float scale[3] = { 1.0f, 1.0f, 1.0f };

        ImGui::DragFloat3("Position", position, 0.1f);
        ImGui::DragFloat3("Rotation", rotation, 0.1f);
        ImGui::DragFloat3("Scale", scale, 0.1f);

        ImGui::End();
    }

    void EditorLayer::DrawMenuBar()
    {
        if (!ImGui::BeginMainMenuBar())
            return;

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene", "Ctrl+N"))
            {
            }

            if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
            {
                OpenScene();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
            {
                SaveScene();
            }

            if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
            {
                SaveSceneAs();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit"))
            {
                Engine::Application::Get().CloseApplication();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::MenuItem("Undo", "Ctrl+Z", false, false);
            ImGui::MenuItem("Redo", "Ctrl+Y", false, false);

            ImGui::Separator();

            ImGui::MenuItem("Duplicate", "Ctrl+D", false, false);
            ImGui::MenuItem("Delete", "Delete", false, false);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Scene Hierarchy", nullptr, true);
            ImGui::MenuItem("Inspector", nullptr, true);
            ImGui::MenuItem("Content Browser", nullptr, true);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem("Reload Shaders");
            ImGui::MenuItem("Asset Registry");

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            ImGui::MenuItem("About");

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    void EditorLayer::SaveSceneAs()
    {
        std::string path = Engine::FileDialogs::SaveFile(
            "Eva Scene (*.eva)\0*.eva\0"
        );

        if (path.empty())
            return;

        if (!std::filesystem::path(path).has_extension())
            path += ".eva";

        m_scenePath = path;

        Engine::SceneSerializer serializer(&m_editorScene);

        if (!serializer.Serialize(m_scenePath))
        {
            APP_ERROR("Failed to save scene {}", m_scenePath.string());
            return;
        }

        APP_INFO("Saved scene {}", m_scenePath.string());
    }

    void EditorLayer::SaveScene()
    {
        if (m_scenePath.empty())
        {
            SaveSceneAs();
            return;
        }

        Engine::SceneSerializer serializer(&m_editorScene);
        serializer.Serialize(m_scenePath);
    }






    void EditorLayer::OpenScene()
    {
        std::string path = Engine::FileDialogs::OpenFile(
            "Eva Scene (*.eva)\0*.eva\0"
        );

        if (path.empty())
            return;

        m_scenePath = path;

        m_editorScene.Clear();

        Engine::SceneSerializer serializer(&m_editorScene);

        if (!serializer.Deserialize(m_scenePath))
        {
            APP_ERROR("Failed to load scene {}", m_scenePath.string());
            return;
        }

        APP_INFO("Loaded scene {}", m_scenePath.string());
    }

    void EditorLayer::OnEvent(Engine::Event& event)
    {
        Engine::EventDispatcher dispatcher(event);

        dispatcher.Dispatch<Engine::MouseScrolledEvent>(
            [this](Engine::MouseScrolledEvent& event)
            {
                if (!m_viewportHovered)
                    return false;

                float zoom = m_editorCamera.GetZoom();
                zoom -= event.GetYOffset() * 0.15f;
                m_editorCamera.SetZoom(zoom);

                return true;
            });
    }

}