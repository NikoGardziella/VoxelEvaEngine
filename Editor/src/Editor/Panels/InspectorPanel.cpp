#include "InspectorPanel.h"

#include "Engine/Scene/Components/Core/TagComponent.h"
#include "Engine/Scene/Components/Core/TransformComponent.h"
#include "Engine/Scene/Components/Rendering/VoxelRendererComponent.h"

#include <imgui.h>

#include <array>
#include <cstring>

namespace Editor
{


    void InspectorPanel::OnImGuiRender()
    {
        ImGui::Begin("Inspector");

        if (!m_selection->SelectedEntity || !m_selection->SelectedEntity.IsValid())
        {
            ImGui::TextDisabled("No entity selected");
            ImGui::End();
            return;
        }

        DrawTagComponent();
        DrawTransformComponent();
        DrawVoxelRendererComponent();

        ImGui::End();
    }

    void InspectorPanel::DrawTagComponent()
    {
        if (!m_selection->SelectedEntity.HasComponent<Engine::TagComponent>())
            return;

        auto& tagComponent = m_selection->SelectedEntity.GetComponent<Engine::TagComponent>();

        std::array<char, 256> buffer{};
        std::strncpy(buffer.data(), tagComponent.Tag.c_str(), buffer.size() - 1);

        if (ImGui::InputText("Name", buffer.data(), buffer.size()))
        {
            tagComponent.Tag = buffer.data();
        }
    }

    void InspectorPanel::DrawTransformComponent()
    {
        if (!m_selection->SelectedEntity.HasComponent<Engine::TransformComponent>())
            return;

        auto& transform = m_selection->SelectedEntity.GetComponent<Engine::TransformComponent>();

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("Translation", &transform.Translation.x, 0.1f);
            ImGui::DragFloat3("Rotation", &transform.Rotation.x, 0.5f);
            ImGui::DragFloat3("Scale", &transform.Scale.x, 0.1f);

            if (transform.Scale.x < 0.001f)
                transform.Scale.x = 0.001f;

            if (transform.Scale.y < 0.001f)
                transform.Scale.y = 0.001f;

            if (transform.Scale.z < 0.001f)
                transform.Scale.z = 0.001f;
        }
    }

    void InspectorPanel::DrawVoxelRendererComponent()
    {
        if (!m_selection->SelectedEntity.HasComponent<Engine::VoxelRendererComponent>())
            return;

        auto& voxelRenderer = m_selection->SelectedEntity.GetComponent<Engine::VoxelRendererComponent>();

        if (ImGui::CollapsingHeader("Voxel Renderer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Voxel Asset Handle: %llu", static_cast<unsigned long long>(voxelRenderer.VoxelAsset));
        }
    }
}