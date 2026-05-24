#pragma once

#include "Voxel.h"

#include <glm/glm.hpp>
#include <array>
#include <cstdint>

namespace Engine
{
    class VoxelChunk
    {
    public:
        static constexpr int SizeX = 16;
        static constexpr int SizeY = 16;
        static constexpr int SizeZ = 16;
        static constexpr int VoxelCount = SizeX * SizeY * SizeZ;

    public:
        VoxelChunk() = default;

        void Clear();
        void Fill(uint16_t type);
        void SetVoxel(int x, int y, int z, uint16_t type);
        const Voxel& GetVoxel(int x, int y, int z) const;

        bool IsInside(int x, int y, int z) const;
        bool IsSolid(int x, int y, int z) const;

        glm::ivec3 GetPosition() const { return m_position; }
        void SetPosition(const glm::ivec3& position) { m_position = position; }


        void SetVoxel(int x, int y, int z, uint16_t type, const glm::u8vec4& color = { 255, 255, 255, 255 });
    private:
        int GetIndex(int x, int y, int z) const;

    private:
        std::array<Voxel, VoxelCount> m_voxels{};
        glm::ivec3 m_position = { 0, 0, 0 };
    };
}