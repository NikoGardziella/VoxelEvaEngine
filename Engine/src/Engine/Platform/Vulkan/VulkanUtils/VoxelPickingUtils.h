#pragma once

#include <glm/glm.hpp>
#include <cstdint>

namespace Engine
{
    namespace VoxelPickingUtils
    {
        static constexpr uint32_t LocalBits = 12;
        static constexpr uint32_t LocalMask = 0xFFF;

        inline bool DecodeVoxelPickID(uint32_t encodedID, uint32_t& chunkIndex, glm::ivec3& localVoxel)
        {
            if (encodedID == 0)
                return false;

            uint32_t raw = encodedID - 1;

            uint32_t localIndex = raw & LocalMask;

            chunkIndex = raw >> LocalBits;

            int x = localIndex % 16;
            int y = (localIndex / 16) % 16;
            int z = localIndex / (16 * 16);

            localVoxel = { x, y, z };

            return true;
        }
    }
}