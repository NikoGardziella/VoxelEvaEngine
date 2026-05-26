#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace Engine
{
    static constexpr uint32_t VOXEL_CHUNK_SIZE = 16;
    static constexpr uint32_t VOXEL_LOCAL_BITS = 12;
    static constexpr uint32_t VOXEL_LOCAL_MASK = 0xFFF;

    inline uint32_t EncodeVoxelPickID(uint32_t chunkIndex, uint32_t localVoxelIndex)
    {
        return ((chunkIndex << VOXEL_LOCAL_BITS) | localVoxelIndex) + 1;
    }

    inline bool DecodeVoxelPickID(uint32_t encodedID, uint32_t& chunkIndex, uint32_t& localVoxelIndex)
    {
        if (encodedID == 0)
            return false;

        uint32_t raw = encodedID - 1;

        localVoxelIndex = raw & VOXEL_LOCAL_MASK;
        chunkIndex = raw >> VOXEL_LOCAL_BITS;

        return true;
    }

    inline uint32_t GetLocalVoxelIndex(uint32_t x, uint32_t y, uint32_t z)
    {
        return x + y * VOXEL_CHUNK_SIZE + z * VOXEL_CHUNK_SIZE * VOXEL_CHUNK_SIZE;
    }

    inline glm::ivec3 DecodeLocalVoxelIndex(uint32_t localVoxelIndex)
    {
        int x = localVoxelIndex % VOXEL_CHUNK_SIZE;
        int y = (localVoxelIndex / VOXEL_CHUNK_SIZE) % VOXEL_CHUNK_SIZE;
        int z = localVoxelIndex / (VOXEL_CHUNK_SIZE * VOXEL_CHUNK_SIZE);

        return { x, y, z };
    }
}