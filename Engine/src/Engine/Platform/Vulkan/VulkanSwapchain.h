#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>

namespace Engine
{
    class VulkanSwapchain
    {
    public:
        VulkanSwapchain(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkSurfaceKHR surface,
            uint32_t width,
            uint32_t height,
            uint32_t graphicsQueueFamily,
            uint32_t presentQueueFamily
        );

        ~VulkanSwapchain();

        VulkanSwapchain(const VulkanSwapchain&) = delete;
        VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;

        VkSwapchainKHR GetSwapchain() const { return m_swapchain; }

        VkFormat GetImageFormat() const { return m_imageFormat; }
        VkExtent2D GetExtent() const { return m_extent; }

        const std::vector<VkImage>& GetImages() const { return m_images; }
        const std::vector<VkImageView>& GetImageViews() const { return m_imageViews; }

    private:
        void CreateSwapchain(
            VkPhysicalDevice physicalDevice,
            VkSurfaceKHR surface,
            uint32_t width,
            uint32_t height,
            uint32_t graphicsQueueFamily,
            uint32_t presentQueueFamily
        );

        void CreateImageViews();

        VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
        VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);
        VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;

        VkFormat m_imageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D m_extent{};

        std::vector<VkImage> m_images;
        std::vector<VkImageView> m_imageViews;
    };
}