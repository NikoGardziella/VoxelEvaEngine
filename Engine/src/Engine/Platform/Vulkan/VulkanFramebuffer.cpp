#include "VulkanFramebuffer.h"

#include <stdexcept>
#include <array>

namespace Engine
{
    VulkanFramebuffer::VulkanFramebuffer(
        VkDevice device,
        VkRenderPass renderPass,
        const std::vector<VkImageView>& swapchainImageViews,
        VkExtent2D swapchainExtent
    )
        : m_device(device)
    {
        CreateFramebuffers(renderPass, swapchainImageViews, swapchainExtent);
    }

    VulkanFramebuffer::~VulkanFramebuffer()
    {
        for (VkFramebuffer framebuffer : m_framebuffers)
        {
            vkDestroyFramebuffer(m_device, framebuffer, nullptr);
        }

        m_framebuffers.clear();
    }

    void VulkanFramebuffer::CreateFramebuffers(
        VkRenderPass renderPass,
        const std::vector<VkImageView>& swapchainImageViews,
        VkExtent2D swapchainExtent
    )
    {
        m_framebuffers.resize(swapchainImageViews.size());

        for (size_t i = 0; i < swapchainImageViews.size(); i++)
        {
            std::array<VkImageView, 1> attachments =
            {
                swapchainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapchainExtent.width;
            framebufferInfo.height = swapchainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create Vulkan framebuffer");
            }
        }
    }
}