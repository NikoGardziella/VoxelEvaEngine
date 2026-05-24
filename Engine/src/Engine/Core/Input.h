#pragma once

#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/MouseCodes.h"

#include <utility>
#include <glm/ext/vector_float2.hpp>

namespace Engine
{
    class Input
    {
    public:
        static void Update();

        static bool IsKeyPressed(KeyCode keyCode);
        static bool IsKeyDown(KeyCode keyCode);
        static bool IsKeyReleased(KeyCode keyCode);

        static bool IsMouseButtonPressed(MouseCode button);
        static bool IsMouseButtonDown(MouseCode button);
        static bool IsMouseButtonReleased(MouseCode button);

        static std::pair<float, float> GetMousePosition();
        static glm::vec2 GetMouseGlmPosition();

        static float GetMouseX();
        static float GetMouseY();
    };
}