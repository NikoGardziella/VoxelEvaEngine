#pragma once

#include "Engine/Scene/Entity.h"

#include <filesystem>
#include "glm/glm.hpp"

namespace Engine
{
    class Scene;
    class AssetManager;
}

namespace Editor
{
    class AssetPlacementTool
    {
    public:
        AssetPlacementTool() = default;
        AssetPlacementTool(Engine::Scene* scene, Engine::AssetManager* assetManager);

        void SetContext(Engine::Scene* scene, Engine::AssetManager* assetManager);

        Engine::Entity CreateEntityFromAsset(const std::filesystem::path& path, const glm::vec3& position = glm::vec3{0});
        Engine::Entity ApplyAssetToEntity(Engine::Entity entity, const std::filesystem::path& path);

        void SetTargetEntity(Engine::Entity entity);
        void ClearTargetEntity();

        bool HasTargetEntity() const;
        Engine::Entity GetTargetEntity() const { return m_targetEntity; }

    private:
        Engine::Entity CreateOrGetTargetEntity(const std::filesystem::path& path);
        bool IsVoxelAsset(const std::filesystem::path& path) const;

    private:
        Engine::Scene* m_scene = nullptr;
        Engine::AssetManager* m_assetManager = nullptr;
        Engine::Entity m_targetEntity;
    };
}