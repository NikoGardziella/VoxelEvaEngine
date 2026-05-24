#pragma once

#include "RenderTarget.h"

#include <vulkan/vulkan.h>
#include "Vulkan/VulkanMeshBuffer.h"

namespace Engine
{
    class VulkanPipeline;

    class GameRenderer
    {
    public:
        GameRenderer() = default;
        ~GameRenderer() = default;

        void Init(VkDevice device);
        void Shutdown();

        void SetMeshBuffer(VulkanMeshBuffer* meshBuffer);
        void SetViewProjection(const glm::mat4& viewProjection);
        void SetPipeline(VulkanPipeline* pipeline);
        void Render(VkCommandBuffer commandBuffer, RenderTarget& target);

    private:
        void DrawMesh(VkCommandBuffer commandBuffer);

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        VulkanPipeline* m_pipeline = nullptr;

        VulkanMeshBuffer* m_meshBuffer = nullptr;
        glm::mat4 m_viewProjection = glm::mat4(1.0f);
    };
}