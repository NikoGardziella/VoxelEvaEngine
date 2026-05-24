#include "Scene.h"

#include "Engine/Scene/Components/Core/IDComponent.h"
#include "Engine/Scene/Components/Core/TagComponent.h"
#include "Engine/Scene/Components/Core/TransformComponent.h"

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

    EntityID Scene::GenerateEntityID()
    {
        return m_nextEntityID++;
    }
}