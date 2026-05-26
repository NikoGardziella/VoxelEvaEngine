#pragma once

#include "Engine/Assets/AssetHandle.h"
#include "Engine/Renderer/Vulkan/VulkanMeshBuffer.h"

#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace Engine
{
    class AssetManager;
    class RenderTarget;
    class Scene;
    class VulkanPipeline;

    class SceneRenderer
    {
    public:
        SceneRenderer() = default;
        ~SceneRenderer();

        SceneRenderer(const SceneRenderer&) = delete;
        SceneRenderer& operator=(const SceneRenderer&) = delete;
        SceneRenderer(SceneRenderer&&) = delete;
        SceneRenderer& operator=(SceneRenderer&&) = delete;

        void Init(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
        void Shutdown();

        void SetPipeline(VulkanPipeline* pipeline);
        void SetPickingPipeline(VulkanPipeline* pipeline);
        void SetViewProjection(const glm::mat4& viewProjection);

        void RenderScene(VkCommandBuffer commandBuffer, RenderTarget& target, Scene& scene, AssetManager& assetManager);

        void RenderScenePicking(VkCommandBuffer commandBuffer, RenderTarget& target, Scene& scene, AssetManager& assetManager);

        void DrawVoxelEntityPicking(VkCommandBuffer commandBuffer, VulkanMeshBuffer& meshBuffer, const glm::mat4& transform);

    private:
        VulkanMeshBuffer* GetOrCreateVoxelMeshBuffer(AssetHandle handle, AssetManager& assetManager);
        void DrawVoxelEntity(VkCommandBuffer commandBuffer, VulkanMeshBuffer& meshBuffer, const glm::mat4& modelMatrix);

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        VkCommandPool m_commandPool = VK_NULL_HANDLE;
        VkQueue m_graphicsQueue = VK_NULL_HANDLE;

        VulkanPipeline* m_pipeline = nullptr;
        VulkanPipeline* m_pickingPipeline = nullptr;
        glm::mat4 m_viewProjection = glm::mat4(1.0f);

        std::unordered_map<AssetHandle, std::unique_ptr<VulkanMeshBuffer>> m_voxelMeshCache;


    };
}