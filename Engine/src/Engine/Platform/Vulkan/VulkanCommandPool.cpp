#include "VulkanCommandPool.h"

#include <stdexcept>

namespace Engine
{
    VulkanCommandPool::VulkanCommandPool(VkDevice device, uint32_t graphicsQueueFamily)
        : m_device(device)
    {
        CreateCommandPool(graphicsQueueFamily);
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        if (m_commandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(m_device, m_commandPool, nullptr);
            m_commandPool = VK_NULL_HANDLE;
        }
    }

    void VulkanCommandPool::CreateCommandPool(uint32_t graphicsQueueFamily)
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = graphicsQueueFamily;

        if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan command pool");
        }
    }
}