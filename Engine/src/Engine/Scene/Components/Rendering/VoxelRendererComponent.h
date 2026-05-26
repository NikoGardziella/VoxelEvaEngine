#pragma once

#include "Engine/Assets/AssetHandle.h"

namespace Engine
{
    struct VoxelRendererComponent
    {
        AssetHandle VoxelAsset = InvalidAssetHandle;

        VoxelRendererComponent() = default;
        VoxelRendererComponent(AssetHandle voxelAsset) : VoxelAsset(voxelAsset)
        {
        }
    };
}