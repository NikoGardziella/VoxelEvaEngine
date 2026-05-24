#pragma once

namespace Engine
{
    template<typename T, typename... Args>
    T& Entity::AddComponent(Args&&... args)
    {
        return m_scene->m_registry.emplace<T>(m_handle, std::forward<Args>(args)...);
    }

    template<typename T>
    T& Entity::GetComponent()
    {
        return m_scene->m_registry.get<T>(m_handle);
    }

    template<typename T>
    const T& Entity::GetComponent() const
    {
        return m_scene->m_registry.get<T>(m_handle);
    }

    template<typename T>
    bool Entity::HasComponent() const
    {
        return m_scene && m_scene->m_registry.all_of<T>(m_handle);
    }

    template<typename T>
    void Entity::RemoveComponent()
    {
        m_scene->m_registry.remove<T>(m_handle);
    }
}