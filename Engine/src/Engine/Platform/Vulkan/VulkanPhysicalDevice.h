#pragma once

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace Engine
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> GraphicsFamily;
        std::optional<uint32_t> PresentFamily;

        bool IsComplete() const
        {
            return GraphicsFamily.has_value() && PresentFamily.has_value();
        }
    };

    class VulkanPhysicalDevice
    {
    public:
        VulkanPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
        ~VulkanPhysicalDevice() = default;

        void Pick(VkInstance instance, VkSurfaceKHR surface);

        VkPhysicalDevice GetHandle() const { return m_physicalDevice; }
        const VkPhysicalDeviceProperties& GetProperties() const { return m_properties; }
        const VkPhysicalDeviceFeatures& GetFeatures() const { return m_features; }
        const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_queueFamilyIndices; }

    private:
        bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

    private:
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties m_properties{};
        VkPhysicalDeviceFeatures m_features{};
        QueueFamilyIndices m_queueFamilyIndices;

        std::vector<const char*> m_requiredDeviceExtensions =
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    };
}