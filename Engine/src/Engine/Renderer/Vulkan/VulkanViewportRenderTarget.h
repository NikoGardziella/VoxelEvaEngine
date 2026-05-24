#pragma once

#include "Engine/Renderer/RenderTarget.h"

#include <vulkan/vulkan.h>
#include <imgui.h>
#include <cstdint>

namespace Engine
{
    class VulkanViewportRenderTarget : public RenderTarget
    {
    public:
        VulkanViewportRenderTarget() = default;
        ~VulkanViewportRenderTarget();
        VulkanViewportRenderTarget(const VulkanViewportRenderTarget&) = delete;
        VulkanViewportRenderTarget& operator=(const VulkanViewportRenderTarget&) = delete;
        VulkanViewportRenderTarget(VulkanViewportRenderTarget&&) = delete;
        VulkanViewportRenderTarget& operator=(VulkanViewportRenderTarget&&) = delete;

        void Init(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamily, VkFormat colorFormat, uint32_t width, uint32_t height);
        void Shutdown();

        void Begin(VkCommandBuffer commandBuffer) override;
        void End(VkCommandBuffer commandBuffer) override;
        void Resize(uint32_t width, uint32_t height) override;

        VkExtent2D GetExtent() const override { return m_extent; }
        VkFormat GetColorFormat() const override { return m_colorFormat; }
        VkRenderPass GetRenderPass() const override { return m_renderPass; }
        VkFramebuffer GetFramebuffer() const override { return m_framebuffer; }

        VkImageView GetImageView() const { return m_colorImageView; }
        VkSampler GetSampler() const { return m_sampler; }
        ImTextureID GetImGuiTextureID() const { return static_cast<ImTextureID>((uint64_t)m_imguiDescriptorSet); }

        void CreateImGuiDescriptor();
    private:
        bool IsValid() const;
        void CreateResources();
        void DestroyResources();
        void CreateRenderPass();
        void CreateColorImage();
        void CreateFramebuffer();
        void CreateDepthImage();
        void CreateSampler();
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

        uint32_t m_graphicsQueueFamily = 0;

        VkFormat m_colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
        VkExtent2D m_extent = { 0, 0 };

        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

        VkImage m_colorImage = VK_NULL_HANDLE;
        VkDeviceMemory m_colorImageMemory = VK_NULL_HANDLE;
        VkImageView m_colorImageView = VK_NULL_HANDLE;
        VkSampler m_sampler = VK_NULL_HANDLE;

        VkDescriptorSet m_imguiDescriptorSet = VK_NULL_HANDLE;

        VkImage m_depthImage = VK_NULL_HANDLE;
        VkDeviceMemory m_depthImageMemory = VK_NULL_HANDLE;
        VkImageView m_depthImageView = VK_NULL_HANDLE;
        VkFormat m_depthFormat = VK_FORMAT_D32_SFLOAT;
    };
}