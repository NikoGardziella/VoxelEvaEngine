
#pragma once

#include "Engine/Assets/Asset.h"
#include "Engine/Voxel/VoxelChunk.h"

namespace Engine
{
    class VoxelAsset : public Asset
    {
    public:
        AssetType GetType() const override { return AssetType::Voxel; }

        VoxelChunk& GetChunk() { return m_chunk; }
        const VoxelChunk& GetChunk() const { return m_chunk; }

    private:
        VoxelChunk m_chunk;
    };
}