#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Engine
{
    class VulkanDevice
    {
    public:
        VulkanDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        ~VulkanDevice();

        VulkanDevice(const VulkanDevice&) = delete;
        VulkanDevice& operator=(const VulkanDevice&) = delete;

        VkDevice GetDevice() const { return m_device; }

        VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
        VkQueue GetPresentQueue() const { return m_presentQueue; }

        uint32_t GetGraphicsQueueFamily() const { return m_graphicsQueueFamily; }
        uint32_t GetPresentQueueFamily() const { return m_presentQueueFamily; }

    private:
        void PickQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        void CreateLogicalDevice(VkPhysicalDevice physicalDevice);

    private:
        VkDevice m_device = VK_NULL_HANDLE;

        VkQueue m_graphicsQueue = VK_NULL_HANDLE;
        VkQueue m_presentQueue = VK_NULL_HANDLE;

        uint32_t m_graphicsQueueFamily = UINT32_MAX;
        uint32_t m_presentQueueFamily = UINT32_MAX;
    };
}