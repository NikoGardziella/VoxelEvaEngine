#pragma once

#include "SceneTypes.h"
#include "Entity.h"

#include <entt/entt.hpp>
#include <string>

namespace Engine
{
    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        Entity CreateEntity(const std::string& name = "Entity");
        Entity CreateEntityWithID(EntityID id, const std::string& name = "Entity");
        void DestroyEntity(Entity entity);

        Entity FindEntityByID(EntityID id);
        bool IsEntityValid(Entity entity) const;

        void OnFixedUpdate(const SceneTickContext& context);

        entt::registry& GetRegistry() { return m_registry; }
        const entt::registry& GetRegistry() const { return m_registry; }

        template<typename... Components>
        auto GetAllEntitiesWith()
        {
            return m_registry.view<Components...>();
        }

    private:
        EntityID GenerateEntityID();

    private:
        entt::registry m_registry;
        EntityID m_nextEntityID = 1;

        friend class Entity;
    };
}

#include "Entity.inl"