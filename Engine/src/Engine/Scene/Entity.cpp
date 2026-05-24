#include "Entity.h"
#include "Scene.h"

namespace Engine
{
    Entity::Entity(entt::entity handle, Scene* scene) : m_handle(handle), m_scene(scene)
    {
    }

    bool Entity::IsValid() const
    {
        return m_scene && m_handle != entt::null && m_scene->m_registry.valid(m_handle);
    }
}