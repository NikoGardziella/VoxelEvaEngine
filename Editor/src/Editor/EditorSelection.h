#pragma once

#include "Engine/Scene/Entity.h"

namespace Editor
{
    struct EditorSelection
    {
        Engine::Entity SelectedEntity;

        void Clear()
        {
            SelectedEntity = Engine::Entity();
        }

        bool HasEntity() const
        {
            return SelectedEntity && SelectedEntity.IsValid();
        }
    };
}