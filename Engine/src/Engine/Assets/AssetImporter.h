#pragma once

#include "Asset.h"

#include <filesystem>
#include <memory>

namespace Engine
{
    class AssetImporter
    {
    public:
        static std::shared_ptr<Asset> ImportAsset(AssetType type, const std::filesystem::path& path);
    };
}