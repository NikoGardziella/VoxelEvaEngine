#include "AssetManager.h"
#include "AssetImporter.h"
#include <functional>

namespace Engine
{
    AssetHandle AssetManager::ImportAsset(const std::filesystem::path& sourcePath, AssetType type)
    {
        std::filesystem::path normalizedPath = NormalizePath(sourcePath);

        if (const AssetMetadata* existingMetadata = FindMetadataByPath(normalizedPath))
            return existingMetadata->Handle;

        AssetHandle handle = GenerateHandle(normalizedPath);

        while (m_assetRegistry.find(handle) != m_assetRegistry.end())
        {
            handle++;
        }

        AssetMetadata metadata{};
        metadata.Handle = handle;
        metadata.Type = type;
        metadata.SourcePath = normalizedPath;
        metadata.Name = normalizedPath.stem().string();

        m_assetRegistry[handle] = metadata;

        return handle;
    }

    bool AssetManager::RemoveAsset(AssetHandle handle)
    {
        return m_assetRegistry.erase(handle) > 0;
    }

    std::shared_ptr<Asset> AssetManager::GetAsset(AssetHandle handle)
    {
        auto loadedIt = m_loadedAssets.find(handle);

        if (loadedIt != m_loadedAssets.end())
            return loadedIt->second;

        const AssetMetadata* metadata = GetMetadata(handle);

        if (!metadata)
            return nullptr;

        std::shared_ptr<Asset> asset = AssetImporter::ImportAsset(metadata->Type, metadata->SourcePath);

        if (!asset)
            return nullptr;

        asset->SetHandle(handle);

        m_loadedAssets[handle] = asset;

        return asset;
    }

    const AssetMetadata* AssetManager::GetMetadata(AssetHandle handle) const
    {
        auto it = m_assetRegistry.find(handle);

        if (it == m_assetRegistry.end())
            return nullptr;

        return &it->second;
    }

    const AssetMetadata* AssetManager::FindMetadataByPath(const std::filesystem::path& sourcePath) const
    {
        std::filesystem::path normalizedPath = NormalizePath(sourcePath);

        for (const auto& [handle, metadata] : m_assetRegistry)
        {
            if (metadata.SourcePath == normalizedPath)
                return &metadata;
        }

        return nullptr;
    }

    bool AssetManager::IsAssetHandleValid(AssetHandle handle) const
    {
        return m_assetRegistry.find(handle) != m_assetRegistry.end();
    }

    bool AssetManager::IsAssetImported(const std::filesystem::path& sourcePath) const
    {
        return FindMetadataByPath(sourcePath) != nullptr;
    }

    AssetHandle AssetManager::GenerateHandle(const std::filesystem::path& sourcePath) const
    {
        std::hash<std::string> hasher;
        AssetHandle handle = static_cast<AssetHandle>(hasher(NormalizePath(sourcePath).string()));

        if (handle == InvalidAssetHandle)
            handle = 1;

        return handle;
    }

    std::filesystem::path AssetManager::NormalizePath(const std::filesystem::path& path) const
    {
        return path.lexically_normal();
    }
}