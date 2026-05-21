#include "Engine/Core/Application.h"

#include "Engine/Core/Events/ApplicationEvent.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Window.h"
#include <GLFW/glfw3.h>
#include "Assert.h"

namespace Engine
{
    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        ENGINE_INFO("Application created");


        VE_CORE_ASSERT(!s_Instance);
        s_Instance = this;

        WindowSpecification specification;
        specification.Title = "VoxelEvaEngine Editor";
        specification.Width = 1280;
        specification.Height = 720;
        specification.VSync = true;

        m_Window.reset(Window::Create(specification));

        m_Window->SetEventCallback(
            [this](Event& event)
            {
                OnEvent(event);
            });
    }

    Application::~Application()
    {
        ENGINE_INFO("Application destroyed");
    }

    void Application::Run()
    {
        ENGINE_INFO("Application running");

        while (m_Running && !m_Window->ShouldClose())
        {
            float time = static_cast<float>(glfwGetTime());
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            for (Layer* layer : m_LayerStack)
            {
                layer->OnUpdate(timestep);
            }

            m_Window->OnUpdate();
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

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            if (event.Handled)
                break;

            (*it)->OnEvent(event);
        }
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_LayerStack.PushOverlay(overlay);
    }

    bool Application::OnWindowClose(WindowCloseEvent& event)
    {
        (void)event;

        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& event)
    {
        ENGINE_INFO("Window resized to {}x{}", event.GetWidth(), event.GetHeight());
        return false;
    }
}