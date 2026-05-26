#include "AssetPlacementTool.h"

#include "Engine/Assets/AssetManager.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components/Core/TagComponent.h"
#include "Engine/Scene/Components/Rendering/VoxelRendererComponent.h"
#include <Engine/Scene/Components/Core/TransformComponent.h>

namespace Editor
{
    AssetPlacementTool::AssetPlacementTool(Engine::Scene* scene, Engine::AssetManager* assetManager) : m_scene(scene), m_assetManager(assetManager)
    {
    }

    void AssetPlacementTool::SetContext(Engine::Scene* scene, Engine::AssetManager* assetManager)
    {
        m_scene = scene;
        m_assetManager = assetManager;
        m_targetEntity = Engine::Entity();
    }

    Engine::Entity AssetPlacementTool::CreateEntityFromAsset(const std::filesystem::path& path, const glm::vec3& position)
    {
        if (!m_scene || !m_assetManager)
        {
            ENGINE_ERROR("AssetPlacementTool has no scene or asset manager");
            return Engine::Entity();
        }

        if (!IsVoxelAsset(path))
        {
            ENGINE_WARN("Unsupported asset for placement: {}", path.string());
            return Engine::Entity();
        }

        Engine::Entity entity = CreateOrGetTargetEntity(path);
        entity.GetComponent<Engine::TransformComponent>().Translation = position;

        return ApplyAssetToEntity(entity, path);
    }

    Engine::Entity AssetPlacementTool::ApplyAssetToEntity(Engine::Entity entity, const std::filesystem::path& path)
    {
        if (!entity)
        {
            ENGINE_ERROR("Cannot apply asset to invalid entity: {}", path.string());
            return Engine::Entity();
        }

        if (!m_assetManager)
        {
            ENGINE_ERROR("AssetPlacementTool has no asset manager");
            return Engine::Entity();
        }

        if (IsVoxelAsset(path))
        {
            Engine::AssetHandle handle = m_assetManager->ImportAsset(path, Engine::AssetType::Voxel);

            if (entity.HasComponent<Engine::VoxelRendererComponent>())
            {
                entity.GetComponent<Engine::VoxelRendererComponent>().VoxelAsset = handle;
            }
            else
            {
                entity.AddComponent<Engine::VoxelRendererComponent>(handle);
            }

            ENGINE_INFO("Applied voxel asset '{}' to entity", path.string());
            return entity;
        }

        ENGINE_WARN("Unsupported asset type: {}", path.string());
        return Engine::Entity();
    }

    void AssetPlacementTool::SetTargetEntity(Engine::Entity entity)
    {
        m_targetEntity = entity;
    }

    void AssetPlacementTool::ClearTargetEntity()
    {
        m_targetEntity = Engine::Entity();
    }

    bool AssetPlacementTool::HasTargetEntity() const
    {
        return m_targetEntity && m_targetEntity.IsValid();
    }

    Engine::Entity AssetPlacementTool::CreateOrGetTargetEntity(const std::filesystem::path& path)
    {
        if (HasTargetEntity())
            return m_targetEntity;

        std::string name = path.stem().string();

        Engine::Entity entity = m_scene->CreateEntity(name);

        return entity;
    }

    bool AssetPlacementTool::IsVoxelAsset(const std::filesystem::path& path) const
    {
        std::filesystem::path extension = path.extension();

        return extension == ".vox" || extension == ".voxel";
    }
}