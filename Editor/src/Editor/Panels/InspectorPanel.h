#pragma once

#include "Engine/Scene/Entity.h"
#include <Editor/EditorSelection.h>

namespace Editor
{
    class InspectorPanel
    {
    public:
        InspectorPanel() = default;

        void SetSelection(EditorSelection* selection) { m_selection = selection; }


        void OnImGuiRender();

    private:
        void DrawTagComponent();
        void DrawTransformComponent();
        void DrawVoxelRendererComponent();

    private:
        EditorSelection* m_selection = nullptr;
    };
}