#pragma once

#include "Engine/Voxel/VoxelAsset.h"

#include <filesystem>
#include <memory>

namespace Engine
{
    class VoxelAssetLoader
    {
    public:
        static std::shared_ptr<VoxelAsset> Load(const std::filesystem::path& path);
    };
}