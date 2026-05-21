#include "VulkanDevice.h"

#include <vector>
#include <set>
#include <stdexcept>

namespace Engine
{
    VulkanDevice::VulkanDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    {
        PickQueueFamilies(physicalDevice, surface);
        CreateLogicalDevice(physicalDevice);
    }

    VulkanDevice::~VulkanDevice()
    {
        if (m_device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(m_device, nullptr);
            m_device = VK_NULL_HANDLE;
        }
    }

    void VulkanDevice::PickQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                m_graphicsQueueFamily = i;
            }

            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

            if (presentSupport)
            {
                m_presentQueueFamily = i;
            }

            if (m_graphicsQueueFamily != UINT32_MAX && m_presentQueueFamily != UINT32_MAX)
            {
                break;
            }
        }

        if (m_graphicsQueueFamily == UINT32_MAX)
        {
            throw std::runtime_error("Failed to find graphics queue family");
        }

        if (m_presentQueueFamily == UINT32_MAX)
        {
            throw std::runtime_error("Failed to find present queue family");
        }
    }

    void VulkanDevice::CreateLogicalDevice(VkPhysicalDevice physicalDevice)
    {
        std::set<uint32_t> uniqueQueueFamilies =
        {
            m_graphicsQueueFamily,
            m_presentQueueFamily
        };

        float queuePriority = 1.0f;

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        const std::vector<const char*> deviceExtensions =
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan logical device");
        }

        vkGetDeviceQueue(m_device, m_graphicsQueueFamily, 0, &m_graphicsQueue);
        vkGetDeviceQueue(m_device, m_presentQueueFamily, 0, &m_presentQueue);
    }
}