#include "Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components/Core/IDComponent.h"
#include "Engine/Scene/Components/Core/TagComponent.h"
#include "Engine/Scene/Components/Core/TransformComponent.h"
#include "Components/Rendering/VoxelRendererComponent.h"



namespace Engine
{
   


    Entity Scene::CreateEntity(const std::string& name)
    {
        return CreateEntityWithID(GenerateEntityID(), name);
    }

    Entity Scene::CreateEntityWithID(EntityID id, const std::string& name)
    {
        entt::entity handle = m_registry.create();
        Entity entity(handle, this);

        entity.AddComponent<IDComponent>(id);
        entity.AddComponent<TagComponent>(name);
        entity.AddComponent<TransformComponent>();

        if (id >= m_nextEntityID)
            m_nextEntityID = id + 1;

        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        if (!entity.IsValid())
            return;

        m_registry.destroy(entity.GetHandle());
    }

    Entity Scene::FindEntityByID(EntityID id)
    {
        auto view = m_registry.view<IDComponent>();

        for (entt::entity handle : view)
        {
            const IDComponent& idComponent = view.get<IDComponent>(handle);

            if (idComponent.ID == id)
                return Entity(handle, this);
        }

        return Entity();
    }

    bool Scene::IsEntityValid(Entity entity) const
    {
        return entity.GetScene() == this && entity.GetHandle() != entt::null && m_registry.valid(entity.GetHandle());
    }

    void Scene::OnFixedUpdate(const SceneTickContext& context)
    {
        (void)context;
    }

    void Scene::OnRuntimeStart()
    {
    }

    void Scene::OnRuntimeStop()
    {
    }

    void Scene::OnRuntimeUpdate(Timestep ts)
    {
    }

    void Scene::OnEditorUpdate(Timestep ts)
    {
    }

    void Scene::Clear()
    {
        m_registry.clear();
        m_nextEntityID = 1;
    }

    std::unique_ptr<Scene> Scene::Copy() const
    {
        std::unique_ptr<Scene> newScene = std::make_unique<Scene>();

        this->ForEachEntity([&](Entity sourceEntity)
            {
                auto& tag = sourceEntity.GetComponent<TagComponent>();
                auto& entityID = sourceEntity.GetComponent<IDComponent>();

                Entity newEntity = newScene->CreateEntityWithID(entityID.ID, tag.Tag);

                if (sourceEntity.HasComponent<TransformComponent>())
                    newEntity.AddOrReplaceComponent<TransformComponent>(sourceEntity.GetComponent<TransformComponent>());

                if (sourceEntity.HasComponent<VoxelRendererComponent>())
                    newEntity.AddOrReplaceComponent<VoxelRendererComponent>(sourceEntity.GetComponent<VoxelRendererComponent>());
            }
        );

        newScene->m_nextEntityID = m_nextEntityID;

        return newScene;
    }

    EntityID Scene::GenerateEntityID()
    {
        return m_nextEntityID++;
    }
}