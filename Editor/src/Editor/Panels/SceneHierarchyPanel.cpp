#include "SceneHierarchyPanel.h"

#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components/Core/TagComponent.h"

#include <imgui.h>

namespace Editor
{
    SceneHierarchyPanel::SceneHierarchyPanel(Engine::Scene* scene) : m_scene(scene)
    {
    }

    void SceneHierarchyPanel::SetScene(Engine::Scene* scene)
    {
        m_scene = scene;
    }


    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Scene Hierarchy");

        if (m_scene)
        {
            auto view = m_scene->GetRegistry().view<Engine::TagComponent>();

            for (entt::entity entityHandle : view)
            {
                Engine::Entity entity(entityHandle, m_scene);
                DrawEntityNode(entity);
            }

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            {
                m_selection->SelectedEntity = Engine::Entity();
            }

            if (ImGui::BeginPopupContextWindow("SceneHierarchyContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
            {
                if (ImGui::MenuItem("Create Empty Entity"))
                {
                    m_selection->SelectedEntity = m_scene->CreateEntity("Empty Entity");
                }

                ImGui::EndPopup();
            }
        }

        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Engine::Entity entity)
    {
        auto& tag = entity.GetComponent<Engine::TagComponent>().Tag;

        ImGuiTreeNodeFlags flags =
            ((m_selection->SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanAvailWidth;

        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity.GetHandle(), flags, "%s", tag.c_str());

        if (ImGui::IsItemClicked())
        {
            m_selection->SelectedEntity = entity;
        }

        bool entityDeleted = false;

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity"))
            {
                entityDeleted = true;
            }

            ImGui::EndPopup();
        }

        if (opened)
        {
            ImGui::TreePop();
        }

        if (entityDeleted)
        {
            if (m_selection->SelectedEntity == entity)
            {
                m_selection->SelectedEntity = Engine::Entity();
            }

            m_scene->DestroyEntity(entity);
        }
    }
}