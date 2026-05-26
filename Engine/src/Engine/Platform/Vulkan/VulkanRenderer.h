#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "VulkanSwapchain.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffers.h"
#include "VulkanSyncObjects.h"
#include "VulkanPipeline.h"
#include <Engine/Renderer/GameRenderer.h>
#include <Engine/Renderer/Vulkan/VulkanViewportRenderTarget.h>
#include <Engine/Renderer/Vulkan/VulkanMeshBuffer.h>
#include <Engine/Assets/AssetManager.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Renderer/SceneRenderer.h>
#include <Engine/Renderer/ViewportPickResult.h>
#include <Engine/Renderer/VoxelPickID.h>


#include <filesystem>
#include <imgui.h>
#include <Engine/Renderer/Vulkan/VulkanPickingRenderTarget.h>
#include <Engine/Assets/Assethandle.h>

struct GLFWwindow;
namespace Engine
{
    class VulkanContext;

    class VulkanRenderer
    {
    public:
        VulkanRenderer(VulkanContext* context, GLFWwindow* window);
        ~VulkanRenderer();

        VulkanRenderer(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(const VulkanRenderer&) = delete;

        void Init();
        void Shutdown();

        bool BeginFrame();
        void BeginRenderPass();
        void EndRenderPass();
        void EndFrame();
        void Present();

        void RenderScene(Engine::Scene& scene);

        void RequestViewportResize(uint32_t width, uint32_t height);
        void ApplyPendingViewportResize();

        void OnFramebufferResized() { m_framebufferResized = true; }

        void SetViewProjection(const glm::mat4& viewProjection);
        uint32_t ReadPickingPixel(uint32_t x, uint32_t y);
        Engine::ViewportPickResult ReadViewportPick(uint32_t x, uint32_t y);

        void LoadVoxelAsset(const std::filesystem::path& path);
        VkCommandBuffer GetCurrentCommandBuffer() const;
        VkRenderPass GetRenderPass() const { return m_renderPass->GetRenderPass(); }
        uint32_t GetImageCount() const;
        ImTextureID GetViewportTextureID() const;
        VkExtent2D GetSwapchainExtent() const { return m_swapchain->GetExtent(); }
        Engine::VulkanViewportRenderTarget& GetViewportRenderTarget() { return m_viewportRenderTarget; }
        Engine::VulkanPickingRenderTarget& GetPickingRenderTarget() { return m_pickingRenderTarget; }
        Engine::GameRenderer& GetGameRenderer() { return m_gameRenderer; }
        Engine::AssetManager& GetAssetManager() { return  m_assetManager;  }
        void CreatePickingPipeline();
        void BeginPickingRenderPass();
        void RenderScenePicking(Engine::Scene& scene);
        void EndPickingRenderPass();
    private:
        void CreateViewportPipeline();

        void CreateSwapchainResources();
        void DestroySwapchainResources();
        void RecreateSwapchain();


     

    private:
        VulkanContext* m_context = nullptr;
        GLFWwindow* m_window = nullptr;

        std::unique_ptr<VulkanSwapchain> m_swapchain;
        std::unique_ptr<VulkanRenderPass> m_renderPass;
        std::unique_ptr<VulkanFramebuffer> m_framebuffer;
        std::unique_ptr<VulkanCommandPool> m_commandPool;
        std::unique_ptr<VulkanCommandBuffers> m_commandBuffers;
        std::unique_ptr<VulkanSyncObjects> m_syncObjects;

        std::unique_ptr<VulkanPipeline> m_pipeline;
        std::unique_ptr<VulkanPipeline> m_viewportPipeline;
        std::unique_ptr<VulkanPipeline> m_pickingPipeline;


        Engine::GameRenderer m_gameRenderer;
        Engine::VulkanViewportRenderTarget m_viewportRenderTarget;

        uint32_t m_currentFrame = 0;
        uint32_t m_currentImageIndex = 0;

        bool m_frameStarted = false;
        bool m_framebufferResized = false;

        bool m_viewportResizePending = false;
        VkExtent2D m_pendingViewportExtent{};

        VulkanMeshBuffer m_cubeMeshBuffer;

        Engine::AssetManager m_assetManager;
        Engine::VulkanMeshBuffer m_voxelMeshBuffer;

        Engine::SceneRenderer m_sceneRenderer;


        Engine::VulkanPickingRenderTarget m_pickingRenderTarget;



    };
}