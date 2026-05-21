#include "Engine/Platform/WindowsWindow.h"

#include "Engine/Core/Events/ApplicationEvent.h"
#include "Engine/Core/Events/KeyEvent.h"
#include "Engine/Core/Events/MouseEvent.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"
#include <GLFW/glfw3.h>

#include <stdexcept>
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Platform/Vulkan/VulkanContext.h"


namespace Engine
{
    static bool s_GLFWInitialized = false;
    static unsigned int s_GLFWWindowCount = 0;

    WindowsWindow::WindowsWindow(const WindowSpecification& specification)
    {
        Init(specification);
    }

    WindowsWindow::~WindowsWindow()
    {
        Shutdown();
    }

    void WindowsWindow::Init(const WindowSpecification& specification)
    {
        m_Data.Title = specification.Title;
        m_Data.Width = specification.Width;
        m_Data.Height = specification.Height;
        m_Data.VSync = specification.VSync;

        if (!s_GLFWInitialized)
        {
            if (!glfwInit())
                throw std::runtime_error("Failed to initialize GLFW");

            s_GLFWInitialized = true;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_window = glfwCreateWindow(
            static_cast<int>(m_Data.Width),
            static_cast<int>(m_Data.Height),
            m_Data.Title.c_str(),
            nullptr,
            nullptr
        );
        ENGINE_INFO("Created window: {} ({}x{})", m_Data.Title, m_Data.Width, m_Data.Height);
        ENGINE_INFO("GLFW window should close: {0}", glfwWindowShouldClose(m_window));
        VE_CORE_ASSERT(m_window != nullptr, "Failed to create GLFW window");

        if (!m_window)
            throw std::runtime_error("Failed to create GLFW window");



        glfwSetWindowUserPointer(m_window, &m_Data);
        m_context = std::make_unique<VulkanContext>(m_window);
        m_context->Init();
        
        ++s_GLFWWindowCount;


        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
            {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

                data.Width = static_cast<unsigned int>(width);
                data.Height = static_cast<unsigned int>(height);

                if (data.Context)
                {
                    data.Context->OnFramebufferResized();
                }

                WindowResizeEvent event(data.Width, data.Height);
                data.EventCallback(event);
            });
        glfwSetWindowCloseCallback(m_window,
            [](GLFWwindow* window)
            {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

                WindowCloseEvent event;
                data.EventCallback(event);
            });

        glfwSetKeyCallback(m_window,
            [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                (void)scancode;
                (void)mods;

                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

                switch (action)
                {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event(key, false);
                    data.EventCallback(event);
                    break;
                }

                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(key);
                    data.EventCallback(event);
                    break;
                }

                case GLFW_REPEAT:
                {
                    KeyPressedEvent event(key, true);
                    data.EventCallback(event);
                    break;
                }
                }
            });

        glfwSetCursorPosCallback(m_window,
            [](GLFWwindow* window, double x, double y)
            {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

                MouseMovedEvent event(static_cast<float>(x), static_cast<float>(y));
                data.EventCallback(event);
            });

        SetVSync(m_Data.VSync);

        glfwSetMouseButtonCallback(m_window,
            [](GLFWwindow* window, int button, int action, int mods)
            {
                (void)mods;

                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

                switch (action)
                {
                case GLFW_PRESS:
                {
                    MouseButtonPressedEvent event(button);
                    if (data.EventCallback)
                        data.EventCallback(event);
                    break;
                }

                case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent event(button);
                    if (data.EventCallback)
                        data.EventCallback(event);
                    break;
                }
                }
            });

        glfwSetScrollCallback(m_window,
            [](GLFWwindow* window, double xOffset, double yOffset)
            {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

                MouseScrolledEvent event(
                    static_cast<float>(xOffset),
                    static_cast<float>(yOffset)
                );

                if (data.EventCallback)
                    data.EventCallback(event);
            });


    }

    void WindowsWindow::Shutdown()
    {
        if (m_context)
        {
            m_context->Shutdown();
            m_context.reset();
        }

        if (m_window)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;

            if (s_GLFWWindowCount > 0)
                --s_GLFWWindowCount;
        }

        if (s_GLFWInitialized && s_GLFWWindowCount == 0)
        {
            glfwTerminate();
            s_GLFWInitialized = false;
        }
    }

    void WindowsWindow::OnUpdate()
    {
        glfwPollEvents();
        if (m_context->BeginFrame())
        {
            m_context->DrawFrame();
            m_context->EndFrame();
            m_context->SwapBuffers();
        }
        else
        {
            ENGINE_WARN("begin fram failed");
        }
    }

    void WindowsWindow::SetVSync(bool enabled)
    {
        m_Data.VSync = enabled;
    }

    bool WindowsWindow::ShouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }
}