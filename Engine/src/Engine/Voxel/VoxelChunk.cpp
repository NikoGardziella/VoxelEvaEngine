#include "VoxelChunk.h"

namespace Engine
{
    void VoxelChunk::Clear()
    {
        for (Voxel& voxel : m_voxels)
        {
            voxel.Type = 0;
        }
    }

    void VoxelChunk::Fill(uint16_t type)
    {
        for (Voxel& voxel : m_voxels)
        {
            voxel.Type = type;
        }
    }

    void VoxelChunk::SetVoxel(int x, int y, int z, uint16_t type)
    {
        if (!IsInside(x, y, z))
            return;

        m_voxels[GetIndex(x, y, z)].Type = type;
    }

    const Voxel& VoxelChunk::GetVoxel(int x, int y, int z) const
    {
        static Voxel airVoxel{};

        if (!IsInside(x, y, z))
            return airVoxel;

        return m_voxels[GetIndex(x, y, z)];
    }

    bool VoxelChunk::IsInside(int x, int y, int z) const
    {
        return x >= 0 && x < SizeX && y >= 0 && y < SizeY && z >= 0 && z < SizeZ;
    }

    bool VoxelChunk::IsSolid(int x, int y, int z) const
    {
        if (!IsInside(x, y, z))
            return false;

        return !GetVoxel(x, y, z).IsAir();
    }

    void VoxelChunk::SetVoxel(int x, int y, int z, uint16_t type, const glm::u8vec4& color)
    {
        if (!IsInside(x, y, z))
            return;

        Voxel& voxel = m_voxels[GetIndex(x, y, z)];
        voxel.Type = type;
        voxel.Color = color;
    }

    int VoxelChunk::GetIndex(int x, int y, int z) const
    {
        return x + y * SizeX + z * SizeX * SizeY;
    }
}