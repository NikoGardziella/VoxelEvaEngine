#include "Engine/Platform/Vulkan/VulkanPhysicalDevice.h"

#include "Engine/Core/Log.h"

#include <set>
#include <stdexcept>
#include <vector>

namespace Engine
{
    VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
    {
        Pick(instance, surface);
    }


    void VulkanPhysicalDevice::Pick(VkInstance instance, VkSurfaceKHR surface)
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0)
            throw std::runtime_error("Failed to find GPUs with Vulkan support");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        ENGINE_INFO("Found {} Vulkan physical device(s)", deviceCount);

        for (VkPhysicalDevice device : devices)
        {
            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(device, &properties);

            ENGINE_INFO("Checking GPU: {}", properties.deviceName);

            if (IsDeviceSuitable(device, surface))
            {
                m_physicalDevice = device;
                m_properties = properties;
                vkGetPhysicalDeviceFeatures(device, &m_features);
                m_queueFamilyIndices = FindQueueFamilies(device, surface);

                ENGINE_INFO("Selected GPU: {}", m_properties.deviceName);
                return;
            }
        }

        throw std::runtime_error("Failed to find a suitable Vulkan physical device");
    }

    bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device, surface);

        bool extensionsSupported = CheckDeviceExtensionSupport(device);

        return indices.IsComplete() && extensionsSupported;
    }

    QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; ++i)
        {
            const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.GraphicsFamily = i;
            }

            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport)
            {
                indices.PresentFamily = i;
            }

            if (indices.IsComplete())
                break;
        }

        return indices;
    }

    bool VulkanPhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(
            m_requiredDeviceExtensions.begin(),
            m_requiredDeviceExtensions.end()
        );

        for (const VkExtensionProperties& extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }
}