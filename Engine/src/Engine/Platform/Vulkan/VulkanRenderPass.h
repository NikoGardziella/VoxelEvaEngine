#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
    class VulkanRenderPass
    {
    public:
        VulkanRenderPass(VkDevice device, VkFormat swapchainImageFormat);
        ~VulkanRenderPass();

        VulkanRenderPass(const VulkanRenderPass&) = delete;
        VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;

        VkRenderPass GetRenderPass() const { return m_renderPass; }

    private:
        void CreateRenderPass(VkFormat swapchainImageFormat);

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
    };
}