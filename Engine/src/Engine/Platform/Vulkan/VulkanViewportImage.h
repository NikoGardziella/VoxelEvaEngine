#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
    class VulkanContext;

    class VulkanViewportImage
    {
    public:
        VulkanViewportImage() = default;
        ~VulkanViewportImage();

        VulkanViewportImage(const VulkanViewportImage&) = delete;
        VulkanViewportImage& operator=(const VulkanViewportImage&) = delete;

        void Init(VulkanContext* context, VkExtent2D extent, VkFormat format);
        void Shutdown();

        void DestroyImGuiDescriptor();

        void Resize(VkExtent2D extent);

        VkImage GetImage() const { return m_image; }
        VkImageView GetImageView() const { return m_imageView; }
        VkSampler GetSampler() const { return m_sampler; }
        VkDescriptorSet GetImGuiDescriptorSet() const { return m_imguiDescriptorSet; }
        VkExtent2D GetExtent() const { return m_extent; }
        VkFormat GetFormat() const { return m_format; }
        void CreateImGuiDescriptor();

        VkFramebuffer GetFramebuffer() const { return m_framebuffer; }
        VkRenderPass GetRenderPass() const { return m_renderPass; }

    private:
        void CreateImage();
        void CreateImageView();
        void CreateSampler();
        void CreateRenderPass();
        void CreateFramebuffer();

        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    private:
        VulkanContext* m_context = nullptr;

        VkImage m_image = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;
        VkImageView m_imageView = VK_NULL_HANDLE;
        VkSampler m_sampler = VK_NULL_HANDLE;
        VkDescriptorSet m_imguiDescriptorSet = VK_NULL_HANDLE;

        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

        VkExtent2D m_extent{};
        VkFormat m_format = VK_FORMAT_R8G8B8A8_UNORM;
    };
}