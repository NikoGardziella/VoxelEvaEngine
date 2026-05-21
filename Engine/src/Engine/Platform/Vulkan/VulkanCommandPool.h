#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
    class VulkanCommandPool
    {
    public:
        VulkanCommandPool(VkDevice device, uint32_t graphicsQueueFamily);
        ~VulkanCommandPool();

        VulkanCommandPool(const VulkanCommandPool&) = delete;
        VulkanCommandPool& operator=(const VulkanCommandPool&) = delete;

        VkCommandPool GetCommandPool() const { return m_commandPool; }

    private:
        void CreateCommandPool(uint32_t graphicsQueueFamily);

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        VkCommandPool m_commandPool = VK_NULL_HANDLE;
    };
}