#pragma once

#include "SceneTypes.h"

#include <entt/entt.hpp>
#include <string>
#include <memory>
#include "Engine/Scene/Components/Core/IDComponent.h"
#include "Engine/Core/Timestep.h"

namespace Engine
{

    class Entity;
    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

        Entity CreateEntity(const std::string& name = "Entity");
        Entity CreateEntityWithID(EntityID id, const std::string& name = "Entity");
        void DestroyEntity(Entity entity);

        Entity FindEntityByID(EntityID id);
        bool IsEntityValid(Entity entity) const;

        void OnFixedUpdate(const SceneTickContext& context);
        void OnRuntimeStart();
        void OnRuntimeStop();
        void OnRuntimeUpdate(Timestep ts);
        void OnEditorUpdate(Timestep ts);
        void Clear();

        std::unique_ptr<Scene> Copy() const;

        entt::registry& GetRegistry() { return m_registry; }
        const entt::registry& GetRegistry() const { return m_registry; }

        template<typename Func>
        void ForEachEntity(Func&& func)
        {
            auto& storage = m_registry.storage<entt::entity>();

            for (entt::entity entityHandle : storage)
            {
                func(Entity(entityHandle, this));
            }
        }

        template<typename Func>
        void ForEachEntity(Func&& func) const
        {
            auto view = m_registry.view<IDComponent>();

            for (entt::entity entityHandle : view)
            {
                func(Entity(entityHandle, const_cast<Scene*>(this)));
            }
        }

        template<typename... Components, typename Func>
        void ForEachWith(Func&& func)
        {
            auto view = m_registry.view<Components...>();

            for (entt::entity entityHandle : view)
            {
                func(Entity(entityHandle, this), view.get<Components>(entityHandle)...);
            }
        }
    
        

    private:
        EntityID GenerateEntityID();

    private:
        entt::registry m_registry;
        EntityID m_nextEntityID = 1;

        friend class Entity;
    };
}

