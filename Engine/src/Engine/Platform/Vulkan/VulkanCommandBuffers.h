#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Engine
{
    class VulkanCommandBuffers
    {
    public:
        VulkanCommandBuffers(
            VkDevice device,
            VkCommandPool commandPool,
            VkRenderPass renderPass,
            VkPipeline pipeline,
            const std::vector<VkFramebuffer>& framebuffers,
            VkExtent2D extent
        );

        ~VulkanCommandBuffers();

        VulkanCommandBuffers(const VulkanCommandBuffers&) = delete;
        VulkanCommandBuffers& operator=(const VulkanCommandBuffers&) = delete;

        const std::vector<VkCommandBuffer>& GetCommandBuffers() const { return m_commandBuffers; }
        VkCommandBuffer GetCommandBuffer(uint32_t index) const { return m_commandBuffers[index]; }


        void RecordCommandBuffer(uint32_t imageIndex, VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent, VkPipeline pipeline);
    private:
        void AllocateCommandBuffers(uint32_t count);
        void RecordCommandBuffers(VkRenderPass renderPass, VkPipeline pipeline, const std::vector<VkFramebuffer>& framebuffers, VkExtent2D extent);


    private:
        VkDevice m_device = VK_NULL_HANDLE;
        VkCommandPool m_commandPool = VK_NULL_HANDLE;

        std::vector<VkCommandBuffer> m_commandBuffers;
    };
}