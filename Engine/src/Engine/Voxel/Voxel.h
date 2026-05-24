#pragma once

#include <cstdint>
#include "glm/glm.hpp"

namespace Engine
{
    struct Voxel
    {
        uint16_t Type = 0;
        glm::u8vec4 Color = { 255,255,255,255 };
        bool IsAir() const { return Type == 0; }
    };
}