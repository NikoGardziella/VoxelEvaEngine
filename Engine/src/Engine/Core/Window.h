#pragma once

#include "Engine/Core/Event.h"

#include <functional>
#include <string>

struct GLFWwindow;

namespace Engine
{
    struct WindowSpecification
    {
        std::string Title = "VoxelEvaEngine";
        unsigned int Width = 1280;
        unsigned int Height = 720;
        bool VSync = true;
    };

    class Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() = default;

        virtual void OnUpdate() = 0;

        virtual unsigned int GetWidth() const = 0;
        virtual unsigned int GetHeight() const = 0;

        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual bool ShouldClose() const = 0;

        virtual GLFWwindow* GetNativeWindow() const = 0;

        static Window* Create(const WindowSpecification& specification = WindowSpecification());
    };
}