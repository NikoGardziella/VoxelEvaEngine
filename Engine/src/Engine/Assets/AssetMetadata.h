#pragma once

#include "AssetHandle.h"
#include "Asset.h"

#include <filesystem>
#include <string>

namespace Engine
{
    struct AssetMetadata
    {
        AssetHandle Handle = InvalidAssetHandle;
        AssetType Type = AssetType::None;

        std::filesystem::path SourcePath;
        std::string Name;

        bool IsValid() const { return Handle != InvalidAssetHandle && Type != AssetType::None && !SourcePath.empty(); }
    };
}