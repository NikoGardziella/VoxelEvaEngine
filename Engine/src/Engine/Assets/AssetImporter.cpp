#include "AssetImporter.h"

#include "Engine/Assets/Loaders/VoxelAssetLoader.h"

namespace Engine
{
    std::shared_ptr<Asset> AssetImporter::ImportAsset(AssetType type, const std::filesystem::path& path)
    {
        switch (type)
        {
        case AssetType::Voxel:
            return VoxelAssetLoader::Load(path);

        default:
            return nullptr;
        }
    }
}