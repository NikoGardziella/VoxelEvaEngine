#include "VulkanSwapchainRenderTarget.h"

#include <array>

namespace Engine
{
    void VulkanSwapchainRenderTarget::SetCurrentTarget(VkRenderPass renderPass, VkFramebuffer framebuffer, VkFormat colorFormat, VkExtent2D extent)
    {
        m_renderPass = renderPass;
        m_framebuffer = framebuffer;
        m_colorFormat = colorFormat;
        m_extent = extent;
    }

    void VulkanSwapchainRenderTarget::Begin(VkCommandBuffer commandBuffer)
    {
        std::array<VkClearValue, 1> clearValues{};
        clearValues[0].color = { 0.08f, 0.08f, 0.10f, 1.0f };

        VkRenderPassBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.renderPass = m_renderPass;
        beginInfo.framebuffer = m_framebuffer;
        beginInfo.renderArea.offset = { 0, 0 };
        beginInfo.renderArea.extent = m_extent;
        beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        beginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_extent.width);
        viewport.height = static_cast<float>(m_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_extent;

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void VulkanSwapchainRenderTarget::End(VkCommandBuffer commandBuffer)
    {
        vkCmdEndRenderPass(commandBuffer);
    }

    void VulkanSwapchainRenderTarget::Resize(uint32_t width, uint32_t height)
    {
        m_extent = { width, height };
    }
}