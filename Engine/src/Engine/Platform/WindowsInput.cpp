#include "Engine/Core/Input.h"

#include "Engine/Core/Application.h"
#include "Engine/Core/Window.h"

#include <GLFW/glfw3.h>

#include <array>

namespace Engine
{
    namespace
    {
        constexpr int MaxKeys = 512;
        constexpr int MaxMouseButtons = 16;

        std::array<bool, MaxKeys> s_CurrentKeys{};
        std::array<bool, MaxKeys> s_PreviousKeys{};

        std::array<bool, MaxMouseButtons> s_CurrentMouseButtons{};
        std::array<bool, MaxMouseButtons> s_PreviousMouseButtons{};

        bool IsValidKey(KeyCode keyCode)
        {
            return keyCode >= 0 && keyCode < MaxKeys;
        }

        bool IsValidMouseButton(MouseCode button)
        {
            return button >= 0 && button < MaxMouseButtons;
        }

        GLFWwindow* GetGLFWWindow()
        {
            return Application::Get().GetWindow().GetNativeWindow();
        }
    }

    void Input::Update()
    {
        GLFWwindow* window = GetGLFWWindow();

        s_PreviousKeys = s_CurrentKeys;
        s_PreviousMouseButtons = s_CurrentMouseButtons;

        for (int key = 0; key < MaxKeys; ++key)
        {
            int state = glfwGetKey(window, key);
            s_CurrentKeys[key] = state == GLFW_PRESS || state == GLFW_REPEAT;
        }

        for (int button = 0; button < MaxMouseButtons; ++button)
        {
            int state = glfwGetMouseButton(window, button);
            s_CurrentMouseButtons[button] = state == GLFW_PRESS;
        }
    }

    bool Input::IsKeyPressed(KeyCode keyCode)
    {
        if (!IsValidKey(keyCode))
            return false;

        return s_CurrentKeys[keyCode];
    }

    bool Input::IsKeyDown(KeyCode keyCode)
    {
        if (!IsValidKey(keyCode))
            return false;

        return s_CurrentKeys[keyCode] && !s_PreviousKeys[keyCode];
    }

    bool Input::IsKeyReleased(KeyCode keyCode)
    {
        if (!IsValidKey(keyCode))
            return false;

        return !s_CurrentKeys[keyCode] && s_PreviousKeys[keyCode];
    }

    bool Input::IsMouseButtonPressed(MouseCode button)
    {
        if (!IsValidMouseButton(button))
            return false;

        return s_CurrentMouseButtons[button];
    }

    bool Input::IsMouseButtonDown(MouseCode button)
    {
        if (!IsValidMouseButton(button))
            return false;

        return s_CurrentMouseButtons[button] && !s_PreviousMouseButtons[button];
    }

    bool Input::IsMouseButtonReleased(MouseCode button)
    {
        if (!IsValidMouseButton(button))
            return false;

        return !s_CurrentMouseButtons[button] && s_PreviousMouseButtons[button];
    }

    std::pair<float, float> Input::GetMousePosition()
    {
        GLFWwindow* window = GetGLFWWindow();

        double x = 0.0;
        double y = 0.0;
        glfwGetCursorPos(window, &x, &y);

        return { static_cast<float>(x), static_cast<float>(y) };
    }

    float Input::GetMouseX()
    {
        auto [x, y] = GetMousePosition();
        (void)y;
        return x;
    }

    float Input::GetMouseY()
    {
        auto [x, y] = GetMousePosition();
        (void)x;
        return y;
    }
}