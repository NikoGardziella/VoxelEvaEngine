#pragma once

#include "Engine/Scene/Entity.h"
#include <Editor/EditorSelection.h>

namespace Engine
{
    class Scene;
}

namespace Editor
{
    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel() = default;
        explicit SceneHierarchyPanel(Engine::Scene* scene);

        void SetScene(Engine::Scene* scene);

        void SetSelection(EditorSelection* selection) { m_selection = selection; }

        void OnImGuiRender();

    private:
        void DrawEntityNode(Engine::Entity entity);

    private:
        Engine::Scene* m_scene = nullptr;
        EditorSelection* m_selection = nullptr;
    };
}