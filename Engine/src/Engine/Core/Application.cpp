#include "Engine/Core/Application.h"

#include "Engine/Core/Events/ApplicationEvent.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Window.h"
#include <GLFW/glfw3.h>
#include "Assert.h"
#include "Engine/Core/Input.h"
#include "Engine/Platform/Vulkan/VulkanContext.h"
#include "Engine/Platform/Vulkan/VulkanRenderer.h"

namespace Engine
{
    Application* Application::s_instance = nullptr;

    Application::Application()
    {
        ENGINE_INFO("Application created");


        EE_CORE_ASSERT(!s_instance);
        s_instance = this;

        WindowSpecification specification;
        specification.Title = "VoxelEvaEngine Editor";
        specification.Width = 1280;
        specification.Height = 720;
        specification.VSync = true;

        m_window.reset(Window::Create(specification));

        m_window->SetEventCallback(
            [this](Event& event)
            {
                OnEvent(event);
            });

        GraphicsContext* graphicsContext = m_window->GetGraphicsContext();
        VulkanContext* vulkanContext = static_cast<VulkanContext*>(graphicsContext);

        m_renderer = std::make_unique<VulkanRenderer>(vulkanContext, m_window->GetNativeWindow());
        m_renderer->Init();


       // m_renderer->GetViewportRenderTarget().CreateImGuiDescriptor();

    }

    Application::~Application()
    {
        ENGINE_INFO("Application destroyed");
    }

    void Application::Run()
    {
        ENGINE_INFO("Application running");

        while (m_running && !m_window->ShouldClose())
        {
            float time = static_cast<float>(glfwGetTime());
            Timestep timestep = time - m_lastFrameTime;
            m_lastFrameTime = time;

            m_window->OnUpdate();
            Input::Update();

            m_renderer->ApplyPendingViewportResize();

            if (m_renderer->BeginFrame())
            {
                m_renderer->RenderViewport();

                m_renderer->BeginRenderPass();

                for (Layer* layer : m_layerStack)
                {
                    layer->OnUpdate(timestep);
                }

                BeginImGui();

                for (Layer* layer : m_layerStack)
                {
                    layer->OnImGuiRender();
                }

                EndImGui(m_renderer->GetCurrentCommandBuffer());

                m_renderer->EndRenderPass();
                m_renderer->EndFrame();
                m_renderer->Present();
            }
        }
    }

    void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<WindowCloseEvent>(
            [this](WindowCloseEvent& e)
            {
                return OnWindowClose(e);
            });

        dispatcher.Dispatch<WindowResizeEvent>(
            [this](WindowResizeEvent& e)
            {
                return OnWindowResize(e);
            });

        for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it)
        {
            if (event.Handled)
                break;

            (*it)->OnEvent(event);
        }
    }

    void Application::PushLayer(Layer* layer)
    {
        m_layerStack.PushLayer(layer);
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_layerStack.PushOverlay(overlay);
    }

    VulkanContext* Application::GetVulkanContext() const
    {
        GraphicsContext* graphicsContext = m_window->GetGraphicsContext();
        return static_cast<VulkanContext*>(graphicsContext);
    }

    bool Application::OnWindowClose(WindowCloseEvent& event)
    {
        (void)event;

        m_running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& event)
    {
        ENGINE_INFO("Window resized to {}x{}", event.GetWidth(), event.GetHeight());
        return false;
    }
}