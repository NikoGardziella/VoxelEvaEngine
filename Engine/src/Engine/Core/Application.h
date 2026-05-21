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

        static Application& Get() { return *s_instance; }

        Window& GetWindow() { return *m_window; }
        const Window& GetWindow() const { return *m_window; }

    private:
        bool OnWindowClose(WindowCloseEvent& event);
        bool OnWindowResize(WindowResizeEvent& event);

    private:
        std::unique_ptr<Window> m_window;
        bool m_running = true;

        float m_lastFrameTime = 0.0f;
        LayerStack m_layerStack;

        static Application* s_instance;
    };
}