#pragma once

#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/MouseCodes.h"

#include <utility>

namespace Engine
{
    class Input
    {
    public:
        static bool IsKeyPressed(KeyCode keyCode);
        static bool IsMouseButtonPressed(MouseCode button);
        static std::pair<float, float> GetMousePosition();

        static float GetMouseX();
        static float GetMouseY();
    };
}