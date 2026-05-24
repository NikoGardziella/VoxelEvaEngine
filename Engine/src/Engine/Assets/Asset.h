#pragma once

#include "AssetHandle.h"

namespace Engine
{
    enum class AssetType
    {
        None = 0,
        Texture,
        Voxel,
        Mesh,
        Material,
        Animation,
        Shader
    };

    class Asset
    {
    public:
        virtual ~Asset() = default;

        AssetHandle GetHandle() const { return m_handle; }
        void SetHandle(AssetHandle handle) { m_handle = handle; }

        virtual AssetType GetType() const = 0;

    private:
        AssetHandle m_handle = InvalidAssetHandle;
    };
}