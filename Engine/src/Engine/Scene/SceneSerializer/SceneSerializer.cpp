

#include "SceneSerializer.h"

#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components/Core/TagComponent.h"
#include "Engine/Scene/Components/Core/TransformComponent.h"
#include "Engine/Scene/Components/Rendering/VoxelRendererComponent.h"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <Engine/Scene/Components/Core/IDComponent.h>
#include <Engine/Scene/Scene.h>

namespace YAML
{
    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };
}

namespace Engine
{
    static YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& value)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << value.x << value.y << value.z << YAML::EndSeq;
        return out;
    }

    SceneSerializer::SceneSerializer(Scene* scene) : m_scene(scene)
    {
    }

    bool SceneSerializer::Serialize(const std::filesystem::path& path)
    {
        if (!m_scene)
            return false;

        YAML::Emitter out;

        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";

        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        m_scene->ForEachEntity(
            [&](Entity entity)
            {
                if (!entity.HasComponent<TagComponent>())
                    return;

                out << YAML::BeginMap;


                IDComponent& ID = entity.GetComponent<IDComponent>();

                out << YAML::Key << "Entity" << YAML::Value << static_cast<uint64_t>(ID.ID);

                auto& tag = entity.GetComponent<TagComponent>();
                out << YAML::Key << "TagComponent";
                out << YAML::BeginMap;
                out << YAML::Key << "Tag" << YAML::Value << tag.Tag;
                out << YAML::EndMap;

                if (entity.HasComponent<TransformComponent>())
                {
                    auto& transform = entity.GetComponent<TransformComponent>();

                    out << YAML::Key << "TransformComponent";
                    out << YAML::BeginMap;
                    out << YAML::Key << "Translation" << YAML::Value << transform.Translation;
                    out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
                    out << YAML::Key << "Scale" << YAML::Value << transform.Scale;
                    out << YAML::EndMap;
                }

                if (entity.HasComponent<VoxelRendererComponent>())
                {
                    auto& voxelRenderer = entity.GetComponent<VoxelRendererComponent>();

                    out << YAML::Key << "VoxelRendererComponent";
                    out << YAML::BeginMap;
                    out << YAML::Key << "VoxelAsset" << YAML::Value << voxelRenderer.VoxelAsset;
                    out << YAML::EndMap;
                }

                out << YAML::EndMap;
            }
        );

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream file(path);
        file << out.c_str();

        return true;
    }

    bool SceneSerializer::Deserialize(const std::filesystem::path& path)
    {
        if (!m_scene)
            return false;

        YAML::Node data = YAML::LoadFile(path.string());

        if (!data["Scene"])
            return false;

        YAML::Node entities = data["Entities"];

        if (!entities)
            return true;

        for (const auto& entityNode : entities)
        {
            EntityID entityID = static_cast<EntityID>(entityNode["Entity"].as<uint64_t>());

            std::string name = "Entity";

            YAML::Node tagComponent = entityNode["TagComponent"];
            if (tagComponent)
                name = tagComponent["Tag"].as<std::string>();

            Entity entity = m_scene->CreateEntityWithID(entityID, name);

            YAML::Node transformComponent = entityNode["TransformComponent"];
            if (transformComponent)
            {
                auto& transform = entity.GetComponent<TransformComponent>();
                transform.Translation = transformComponent["Translation"].as<glm::vec3>();
                transform.Rotation = transformComponent["Rotation"].as<glm::vec3>();
                transform.Scale = transformComponent["Scale"].as<glm::vec3>();
            }

            YAML::Node voxelRendererComponent = entityNode["VoxelRendererComponent"];
            if (voxelRendererComponent)
            {
                AssetHandle handle = voxelRendererComponent["VoxelAsset"].as<AssetHandle>();
                entity.AddOrReplaceComponent<VoxelRendererComponent>(handle);
            }
        }

        return true;
    }
}