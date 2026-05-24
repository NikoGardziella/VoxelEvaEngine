#pragma once

#include "Engine/Core/Event.h"
#include "Engine/Core/LayerStack.h"

#include <memory>

#include "vulkan/vulkan.h"

namespace Engine
{
    class Window;
    class WindowCloseEvent;
    class WindowResizeEvent;
    class VulkanRenderer;
    class VulkanContext;

    class Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();
        void OnEvent(Event& event);
        virtual void BeginImGui() {}
        virtual void EndImGui(VkCommandBuffer commandBuffer) { (void)commandBuffer; }
        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        static Application& Get() { return *s_instance; }

        Window& GetWindow() { return *m_window; }
        const Window& GetWindow() const { return *m_window; }
        VulkanContext* GetVulkanContext() const;

        VulkanRenderer& GetRenderer() { return *m_renderer; }
        const VulkanRenderer& GetRenderer() const { return *m_renderer; }
    private:

        bool OnWindowClose(WindowCloseEvent& event);
        bool OnWindowResize(WindowResizeEvent& event);

    private:
        std::unique_ptr<Window> m_window;
        bool m_running = true;

        float m_lastFrameTime = 0.0f;
        LayerStack m_layerStack;
        std::unique_ptr<VulkanRenderer> m_renderer;
 

        static Application* s_instance;
    };
}