#pragma once

#include "Engine/Core/Window.h"
#include <memory>

namespace Engine
{
    class GraphicsContext;
    class WindowsWindow final : public Window
    {

    public:
        explicit WindowsWindow(const WindowSpecification& specification);
        ~WindowsWindow() override;

        void OnUpdate() override;

        unsigned int GetWidth() const override { return m_Data.Width; }
        unsigned int GetHeight() const override { return m_Data.Height; }

        void SetEventCallback(const EventCallbackFn& callback) override
        {
            m_Data.EventCallback = callback;
        }

        void SetVSync(bool enabled) override;
        bool IsVSync() const override { return m_Data.VSync; }

        bool ShouldClose() const override;

        GLFWwindow* GetNativeWindow() const override { return m_window; }
        GraphicsContext* GetGraphicsContext() const override { return m_context.get(); }
    private:
        void Init(const WindowSpecification& specification);
        void Shutdown();

    private:
        GLFWwindow* m_window = nullptr;
        std::unique_ptr<GraphicsContext> m_context;

        struct WindowData
        {
            std::string Title;
            unsigned int Width = 0;
            unsigned int Height = 0;
            bool VSync = true;
            GraphicsContext* Context = nullptr;
            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    };
}