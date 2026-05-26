#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace Engine
{
    struct ViewportPickResult
    {
        bool Hit = false;
        uint32_t RawID = 0;
        uint32_t ChunkIndex = 0;
        uint32_t LocalVoxelIndex = 0;
        glm::ivec3 LocalVoxel = { 0, 0, 0 };
        glm::ivec3 WorldVoxel = { 0, 0, 0 };
    };
}