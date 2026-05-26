#pragma once

#include "Engine/Renderer/RenderTarget.h"

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Engine
{
    class VulkanPickingRenderTarget : public RenderTarget
    {
    public:
        VulkanPickingRenderTarget() = default;
        ~VulkanPickingRenderTarget();

        VulkanPickingRenderTarget(const VulkanPickingRenderTarget&) = delete;
        VulkanPickingRenderTarget& operator=(const VulkanPickingRenderTarget&) = delete;
        VulkanPickingRenderTarget(VulkanPickingRenderTarget&&) = delete;
        VulkanPickingRenderTarget& operator=(VulkanPickingRenderTarget&&) = delete;

        void Init(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat colorFormat, uint32_t width, uint32_t height);
        void Shutdown();

        void Begin(VkCommandBuffer commandBuffer) override;
        void End(VkCommandBuffer commandBuffer) override;
        void Resize(uint32_t width, uint32_t height) override;

        VkExtent2D GetExtent() const override { return m_extent; }
        VkFormat GetColorFormat() const override { return m_colorFormat; }
        VkRenderPass GetRenderPass() const override { return m_renderPass; }
        VkFramebuffer GetFramebuffer() const override { return m_framebuffer; }

        VkImage GetImage() const { return m_colorImage; }
        VkImageView GetImageView() const { return m_colorImageView; }

    private:
        bool IsValid() const;
        void CreateResources();
        void DestroyResources();
        void CreateRenderPass();
        void CreateColorImage();
        void CreateFramebuffer();
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

        VkFormat m_colorFormat = VK_FORMAT_R32_UINT;
        VkExtent2D m_extent = { 0, 0 };

        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

        VkImage m_colorImage = VK_NULL_HANDLE;
        VkDeviceMemory m_colorImageMemory = VK_NULL_HANDLE;
        VkImageView m_colorImageView = VK_NULL_HANDLE;
    };
}