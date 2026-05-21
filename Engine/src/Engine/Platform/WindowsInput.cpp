#include "Engine/Core/Input.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Window.h"

#include <GLFW/glfw3.h>

namespace Engine
{
    bool Input::IsKeyPressed(int keyCode)
    {
        GLFWwindow* window = Application::Get().GetWindow().GetNativeWindow();

        int state = glfwGetKey(window, keyCode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonPressed(int button)
    {
        GLFWwindow* window = Application::Get().GetWindow().GetNativeWindow();

        int state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    std::pair<float, float> Input::GetMousePosition()
    {
        GLFWwindow* window = Application::Get().GetWindow().GetNativeWindow();

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