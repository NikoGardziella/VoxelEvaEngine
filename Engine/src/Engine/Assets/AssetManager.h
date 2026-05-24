#pragma once

#include "AssetMetadata.h"

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace Engine
{
    class AssetManager
    {
    public:
        AssetManager() = default;

        AssetHandle ImportAsset(const std::filesystem::path& sourcePath, AssetType type);
        bool RemoveAsset(AssetHandle handle);

        const AssetMetadata* GetMetadata(AssetHandle handle) const;
        const AssetMetadata* FindMetadataByPath(const std::filesystem::path& sourcePath) const;

        bool IsAssetHandleValid(AssetHandle handle) const;
        bool IsAssetImported(const std::filesystem::path& sourcePath) const;

        const std::unordered_map<AssetHandle, AssetMetadata>& GetAssetRegistry() const { return m_assetRegistry; }
        std::shared_ptr<Asset> GetAsset(AssetHandle handle);
    private:
        AssetHandle GenerateHandle(const std::filesystem::path& sourcePath) const;
        std::filesystem::path NormalizePath(const std::filesystem::path& path) const;

    private:
        std::unordered_map<AssetHandle, AssetMetadata> m_assetRegistry;
        std::unordered_map<AssetHandle, std::shared_ptr<Asset>> m_loadedAssets;
    };
}