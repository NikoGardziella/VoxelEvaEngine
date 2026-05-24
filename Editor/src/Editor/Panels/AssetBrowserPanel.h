#pragma once

#include <filesystem>
#include <functional>

namespace Editor
{
    class AssetBrowserPanel
    {
    public:
        using AssetSelectedCallback = std::function<void(const std::filesystem::path&)>;

    public:
        AssetBrowserPanel();
        explicit AssetBrowserPanel(const std::filesystem::path& assetRoot);

        void OnImGuiRender();

        void SetAssetRoot(const std::filesystem::path& assetRoot);
        void SetAssetSelectedCallback(AssetSelectedCallback callback);

    private:
        void DrawDirectory(const std::filesystem::path& directory);
        bool IsSupportedAsset(const std::filesystem::path& path) const;

    private:
        std::filesystem::path m_assetRoot;
        std::filesystem::path m_currentDirectory;

        AssetSelectedCallback m_assetSelectedCallback;
    };
}