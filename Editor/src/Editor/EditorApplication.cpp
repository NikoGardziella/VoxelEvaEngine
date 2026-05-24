#include "EditorApplication.h"

#include "EditorLayer.h"

namespace Editor
{
    EditorApplication::EditorApplication()
    {
        m_imguiLayer.OnAttach(GetVulkanContext(), &GetRenderer());

        //GetRenderer().GetViewportImage().CreateImGuiDescriptor();
        PushLayer(new EditorLayer(&GetRenderer()));
    }

    EditorApplication::~EditorApplication()
    {
        m_imguiLayer.OnDetach();
    }

    void EditorApplication::BeginImGui()
    {
        m_imguiLayer.Begin();
    }

    void EditorApplication::EndImGui(VkCommandBuffer commandBuffer)
    {
        m_imguiLayer.End(commandBuffer);
    }
}

Engine::Application* CreateApplication()
{
    return new Editor::EditorApplication();
}