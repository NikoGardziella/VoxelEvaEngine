#include "VulkanSwapchain.h"

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <limits>

namespace Engine
{
    VulkanSwapchain::VulkanSwapchain(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VkSurfaceKHR surface,
        uint32_t width,
        uint32_t height,
        uint32_t graphicsQueueFamily,
        uint32_t presentQueueFamily
    )
        : m_device(device)
    {
        CreateSwapchain(
            physicalDevice,
            surface,
            width,
            height,
            graphicsQueueFamily,
            presentQueueFamily
        );

        CreateImageViews();
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        for (VkImageView imageView : m_imageViews)
        {
            vkDestroyImageView(m_device, imageView, nullptr);
        }

        m_imageViews.clear();

        if (m_swapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
            m_swapchain = VK_NULL_HANDLE;
        }
    }

    void VulkanSwapchain::CreateSwapchain(
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        uint32_t width,
        uint32_t height,
        uint32_t graphicsQueueFamily,
        uint32_t presentQueueFamily
    )
    {
        VkSurfaceCapabilitiesKHR capabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

        if (formatCount == 0)
        {
            throw std::runtime_error("Surface has no available formats");
        }

        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

        if (presentModeCount == 0)
        {
            throw std::runtime_error("Surface has no available present modes");
        }

        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

        VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(formats);
        VkPresentModeKHR presentMode = ChoosePresentMode(presentModes);
        VkExtent2D extent = ChooseExtent(capabilities, width, height);

        uint32_t imageCount = capabilities.minImageCount + 1;

        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        {
            imageCount = capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        uint32_t queueFamilyIndices[] =
        {
            graphicsQueueFamily,
            presentQueueFamily
        };

        if (graphicsQueueFamily != presentQueueFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan swapchain");
        }

        vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
        m_images.resize(imageCount);
        vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_images.data());

        m_imageFormat = surfaceFormat.format;
        m_extent = extent;
    }

    void VulkanSwapchain::CreateImageViews()
    {
        m_imageViews.resize(m_images.size());

        for (size_t i = 0; i < m_images.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_imageFormat;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_device, &createInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create Vulkan swapchain image view");
            }
        }
    }

    VkSurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
    {
        for (const VkSurfaceFormatKHR& availableFormat : formats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return formats[0];
    }

    VkPresentModeKHR VulkanSwapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes)
    {
        for (VkPresentModeKHR availablePresentMode : presentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanSwapchain::ChooseExtent(
        const VkSurfaceCapabilitiesKHR& capabilities,
        uint32_t width,
        uint32_t height
    )
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }

        VkExtent2D actualExtent{};
        actualExtent.width = width;
        actualExtent.height = height;

        actualExtent.width = std::clamp(
            actualExtent.width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width
        );

        actualExtent.height = std::clamp(
            actualExtent.height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height
        );

        return actualExtent;
    }
}