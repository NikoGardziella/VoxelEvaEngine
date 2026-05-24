#pragma once

#include <Engine/Core/Application.h>

#include "EditorImGuiLayer.h"
#include <vulkan/vulkan_core.h>

namespace Editor
{
    class EditorApplication : public Engine::Application
    {
    public:
        EditorApplication();
        ~EditorApplication() override;

    protected:
        void BeginImGui() override;
        void EndImGui(VkCommandBuffer commandBuffer) override;

    private:
        EditorImGuiLayer m_imguiLayer;
    };
}