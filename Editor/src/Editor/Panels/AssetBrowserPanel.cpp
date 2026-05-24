#include "AssetBrowserPanel.h"

#include "Engine/Core/Log.h"

#include <imgui.h>

namespace Editor
{
    AssetBrowserPanel::AssetBrowserPanel()
    {
        SetAssetRoot(VE_ENGINE_ASSET_DIR);
    }

    AssetBrowserPanel::AssetBrowserPanel(const std::filesystem::path& assetRoot)
    {
        SetAssetRoot(assetRoot);
    }

    void AssetBrowserPanel::SetAssetRoot(const std::filesystem::path& assetRoot)
    {
        m_assetRoot = assetRoot;
        m_currentDirectory = assetRoot;
    }

    void AssetBrowserPanel::SetAssetSelectedCallback(AssetSelectedCallback callback)
    {
        m_assetSelectedCallback = callback;
    }

    void AssetBrowserPanel::OnImGuiRender()
    {
        ImGui::Begin("Asset Browser");

        ImGui::Text("Root: %s", m_assetRoot.string().c_str());
        ImGui::Text("Current: %s", m_currentDirectory.string().c_str());

        if (m_currentDirectory != m_assetRoot)
        {
            if (ImGui::Button("<- Back"))
            {
                m_currentDirectory = m_currentDirectory.parent_path();
            }
        }

        ImGui::Separator();

        if (!std::filesystem::exists(m_currentDirectory))
        {
            ImGui::TextColored(ImVec4(1.0f, 0.25f, 0.25f, 1.0f), "Directory does not exist");
            ImGui::End();
            return;
        }

        DrawDirectory(m_currentDirectory);

        ImGui::End();
    }

    void AssetBrowserPanel::DrawDirectory(const std::filesystem::path& directory)
    {
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory))
        {
            const std::filesystem::path& path = entry.path();
            std::string name = path.filename().string();

            if (entry.is_directory())
            {
                std::string label = "[Folder] " + name;

                if (ImGui::Selectable(label.c_str(), false))
                {
                    m_currentDirectory = path;
                }

                continue;
            }

            bool supported = IsSupportedAsset(path);
            std::string label = supported ? "[Asset] " + name : "[File] " + name;

            if (ImGui::Selectable(label.c_str(), false))
            {
                if (supported && m_assetSelectedCallback)
                {
                    ENGINE_INFO("Asset selected: {}", path.string());
                    m_assetSelectedCallback(path);
                }
            }
        }
    }

    bool AssetBrowserPanel::IsSupportedAsset(const std::filesystem::path& path) const
    {
        std::filesystem::path extension = path.extension();

        return extension == ".vox" ||
            extension == ".voxel" ||
            extension == ".png" ||
            extension == ".jpg" ||
            extension == ".jpeg" ||
            extension == ".glsl";
    }
}