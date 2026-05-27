#pragma once

#include "Engine/Scene/Scene.h"

#include <filesystem>

namespace Engine
{
    class SceneSerializer
    {
    public:
        SceneSerializer(Scene* scene);

        bool Serialize(const std::filesystem::path& path);
        bool Deserialize(const std::filesystem::path& path);

    private:
        Scene* m_scene = nullptr;
    };
}