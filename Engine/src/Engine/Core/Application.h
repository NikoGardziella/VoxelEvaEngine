#pragma once

#include "Engine/Core/Event.h"
#include "Engine/Core/LayerStack.h"

#include <memory>

namespace Engine
{
    class Window;
    class WindowCloseEvent;
    class WindowResizeEvent;

    class Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();
        void OnEvent(Event& event);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        static Application& Get() { return *s_Instance; }

        Window& GetWindow() { return *m_Window; }
        const Window& GetWindow() const { return *m_Window; }

    private:
        bool OnWindowClose(WindowCloseEvent& event);
        bool OnWindowResize(WindowResizeEvent& event);

    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;

        float m_LastFrameTime = 0.0f;
        LayerStack m_LayerStack;

        static Application* s_Instance;
    };
}