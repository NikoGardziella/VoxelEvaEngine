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



namespace Editor {



    EditorLayer::EditorLayer(Engine::VulkanRenderer* renderer) : Engine::Layer("EditorLayer"), m_renderer(renderer)
    {
        m_assetBrowserPanel.SetAssetSelectedCallback([this](const std::filesystem::path& path)
            {
                if (m_renderer)
                {
                    if (path.extension() == ".vox" || path.extension() == ".voxel")
                    {
                        m_renderer->LoadVoxelAsset(path);
                    }
                }
            });

    }

    EditorLayer::~EditorLayer()
    {
    }

    void EditorLayer::OnAttach()
    {

        
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

        ImVec2 mouse = ImGui::GetMousePos();

        float localX = mouse.x - m_viewportBoundsMin.x;
        float localY = mouse.y - m_viewportBoundsMin.y;

        bool mouseInside =
            localX >= 0.0f &&
            localY >= 0.0f &&
            localX < (m_viewportBoundsMax.x - m_viewportBoundsMin.x) &&
            localY < (m_viewportBoundsMax.y - m_viewportBoundsMin.y);


        m_editorCamera.OnUpdate(ts, m_viewportPanel.IsHovered());
        m_renderer->GetGameRenderer().SetViewProjection(m_editorCamera.GetViewProjection());

       
    }

    void EditorLayer::OnImGuiRender()
    {
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

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::MenuItem("New Scene");
                ImGui::MenuItem("Open Scene");
                ImGui::MenuItem("Save Scene");
                ImGui::Separator();
                ImGui::MenuItem("Exit");
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

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


       // DrawViewportPanel();
        DrawSceneHierarchyPanel();
        DrawInspectorPanel();
        //DrawConsolePanel();
        m_viewportPanel.OnImGuiRender();

        glm::vec2 viewportSize = m_viewportPanel.GetSize();

        if (viewportSize.x > 0.0f && viewportSize.y > 0.0f)
        {
            m_renderer->RequestViewportResize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
            m_editorCamera.OnResize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
        }

        

    }

    

    void EditorLayer::DrawSceneHierarchyPanel()
    {
        ImGui::Begin("Scene Hierarchy");

        ImGui::Selectable("Camera");
        ImGui::Selectable("Triangle");
        ImGui::Selectable("Light");

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