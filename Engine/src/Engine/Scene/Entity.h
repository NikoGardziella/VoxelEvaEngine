#pragma once

#include <entt/entt.hpp>

namespace Engine
{
    class Scene;

    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args);

        template<typename T>
        T& GetComponent();

        template<typename T>
        const T& GetComponent() const;

        template<typename T>
        bool HasComponent() const;

        template<typename T>
        void RemoveComponent();

        bool IsValid() const;
        entt::entity GetHandle() const { return m_handle; }

        operator bool() const { return IsValid(); }
        operator entt::entity() const { return m_handle; }
        bool operator==(const Entity& other) const { return m_handle == other.m_handle && m_scene == other.m_scene; }
        bool operator!=(const Entity& other) const { return !(*this == other); }

        Scene* GetScene() const { return m_scene; }

    private:
        entt::entity m_handle = entt::null;
        Scene* m_scene = nullptr;
    };
}
#include "Engine/Scene/Entity.inl"