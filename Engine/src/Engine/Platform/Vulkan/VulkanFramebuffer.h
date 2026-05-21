#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Engine
{
    class VulkanFramebuffer
    {
    public:
        VulkanFramebuffer(
            VkDevice device,
            VkRenderPass renderPass,
            const std::vector<VkImageView>& swapchainImageViews,
            VkExtent2D swapchainExtent
        );

        ~VulkanFramebuffer();

        VulkanFramebuffer(const VulkanFramebuffer&) = delete;
        VulkanFramebuffer& operator=(const VulkanFramebuffer&) = delete;

        const std::vector<VkFramebuffer>& GetFramebuffers() const { return m_framebuffers; }

    private:
        void CreateFramebuffers(
            VkRenderPass renderPass,
            const std::vector<VkImageView>& swapchainImageViews,
            VkExtent2D swapchainExtent
        );

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        std::vector<VkFramebuffer> m_framebuffers;
    };
}