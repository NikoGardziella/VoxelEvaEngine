#pragma once

#include "Engine/Renderer/RenderTarget.h"

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Engine
{
    class VulkanSwapchainRenderTarget : public RenderTarget
    {
    public:
        VulkanSwapchainRenderTarget() = default;

        void SetCurrentTarget(VkRenderPass renderPass, VkFramebuffer framebuffer, VkFormat colorFormat, VkExtent2D extent);

        void Begin(VkCommandBuffer commandBuffer) override;
        void End(VkCommandBuffer commandBuffer) override;
        void Resize(uint32_t width, uint32_t height) override;

        VkExtent2D GetExtent() const override { return m_extent; }
        VkFormat GetColorFormat() const override { return m_colorFormat; }
        VkRenderPass GetRenderPass() const override { return m_renderPass; }
        VkFramebuffer GetFramebuffer() const override { return m_framebuffer; }

    private:
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
        VkFormat m_colorFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D m_extent = { 0, 0 };
    };
}