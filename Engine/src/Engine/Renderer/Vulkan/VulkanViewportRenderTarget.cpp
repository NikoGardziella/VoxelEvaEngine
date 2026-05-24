#include "VulkanViewportRenderTarget.h"

#include <backends/imgui_impl_vulkan.h>
#include <array>
#include <stdexcept>
#include <Engine/Core/Log.h>

namespace Engine
{
    VulkanViewportRenderTarget::~VulkanViewportRenderTarget()
    {
        Shutdown();
    }

    void VulkanViewportRenderTarget::Init(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamily, VkFormat colorFormat, uint32_t width, uint32_t height)
    {
        m_device = device;
        m_physicalDevice = physicalDevice;
        m_graphicsQueueFamily = graphicsQueueFamily;
        m_colorFormat = colorFormat;
        m_extent = { width, height };

        CreateResources();


    }

    void VulkanViewportRenderTarget::Shutdown()
    {
        DestroyResources();

       
        if (m_depthImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(m_device, m_depthImageView, nullptr);
            m_depthImageView = VK_NULL_HANDLE;
        }

        if (m_depthImage != VK_NULL_HANDLE)
        {
            vkDestroyImage(m_device, m_depthImage, nullptr);
            m_depthImage = VK_NULL_HANDLE;
        }

        if (m_depthImageMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(m_device, m_depthImageMemory, nullptr);
            m_depthImageMemory = VK_NULL_HANDLE;
        }

        m_device = VK_NULL_HANDLE;
        m_physicalDevice = VK_NULL_HANDLE;
        m_graphicsQueueFamily = 0;

    }

    void VulkanViewportRenderTarget::Begin(VkCommandBuffer commandBuffer)
    {
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.05f, 0.05f, 0.07f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

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

    void VulkanViewportRenderTarget::End(VkCommandBuffer commandBuffer)
    {
        vkCmdEndRenderPass(commandBuffer);
    }

    void VulkanViewportRenderTarget::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;

        if (m_extent.width == width && m_extent.height == height)
            return;

        vkDeviceWaitIdle(m_device);

        DestroyResources();

        m_extent = { width, height };

        CreateResources();
    }


    bool VulkanViewportRenderTarget::IsValid() const
    {
        return m_renderPass != VK_NULL_HANDLE &&
            m_framebuffer != VK_NULL_HANDLE &&
            m_extent.width > 0 &&
            m_extent.height > 0;
    }

    void VulkanViewportRenderTarget::CreateResources()
    {
        CreateRenderPass();
        CreateColorImage();
        CreateDepthImage();
        CreateSampler();
        CreateFramebuffer();

        ENGINE_INFO("Created viewport renderPass={}, framebuffer={}", (void*)m_renderPass, (void*)m_framebuffer);
    }

    void VulkanViewportRenderTarget::DestroyResources()
    {
        ENGINE_WARN("Destroying viewport renderPass={}, framebuffer={}", (void*)m_renderPass, (void*)m_framebuffer);
        

        if (m_sampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(m_device, m_sampler, nullptr);
            m_sampler = VK_NULL_HANDLE;
        }

        if (m_framebuffer != VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
            m_framebuffer = VK_NULL_HANDLE;
        }

        if (m_colorImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(m_device, m_colorImageView, nullptr);
            m_colorImageView = VK_NULL_HANDLE;
        }

        if (m_colorImage != VK_NULL_HANDLE)
        {
            vkDestroyImage(m_device, m_colorImage, nullptr);
            m_colorImage = VK_NULL_HANDLE;
        }

        if (m_colorImageMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(m_device, m_colorImageMemory, nullptr);
            m_colorImageMemory = VK_NULL_HANDLE;
        }

        if (m_renderPass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(m_device, m_renderPass, nullptr);
            m_renderPass = VK_NULL_HANDLE;
        }
    }

    void VulkanViewportRenderTarget::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_colorFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = m_depthFormat;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;


        if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
            throw std::runtime_error("Failed to create viewport render pass");
    }

    void VulkanViewportRenderTarget::CreateColorImage()
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = m_extent.width;
        imageInfo.extent.height = m_extent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = m_colorFormat;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(m_device, &imageInfo, nullptr, &m_colorImage) != VK_SUCCESS)
            throw std::runtime_error("Failed to create viewport color image");

        VkMemoryRequirements memoryRequirements{};
        vkGetImageMemoryRequirements(m_device, m_colorImage, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_colorImageMemory) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate viewport color image memory");

        vkBindImageMemory(m_device, m_colorImage, m_colorImageMemory, 0);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_colorImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_colorFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_device, &viewInfo, nullptr, &m_colorImageView) != VK_SUCCESS)
            throw std::runtime_error("Failed to create viewport color image view");
    }

    void VulkanViewportRenderTarget::CreateFramebuffer()
    {
        VkImageView attachments[] = { m_colorImageView, m_depthImageView };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;

        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.width = m_extent.width;
        framebufferInfo.height = m_extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffer) != VK_SUCCESS)
            throw std::runtime_error("Failed to create viewport framebuffer");
    }

    void VulkanViewportRenderTarget::CreateDepthImage()
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = m_extent.width;
        imageInfo.extent.height = m_extent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = m_depthFormat;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(m_device, &imageInfo, nullptr, &m_depthImage) != VK_SUCCESS)
            throw std::runtime_error("Failed to create viewport depth image");

        VkMemoryRequirements memoryRequirements{};
        vkGetImageMemoryRequirements(m_device, m_depthImage, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_depthImageMemory) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate viewport depth image memory");

        vkBindImageMemory(m_device, m_depthImage, m_depthImageMemory, 0);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_depthImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_depthFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_device, &viewInfo, nullptr, &m_depthImageView) != VK_SUCCESS)
            throw std::runtime_error("Failed to create viewport depth image view");
    }

    void VulkanViewportRenderTarget::CreateImGuiDescriptor()
    {
        if (m_imguiDescriptorSet != VK_NULL_HANDLE)
            return;

        if (m_sampler == VK_NULL_HANDLE || m_colorImageView == VK_NULL_HANDLE)
            return;

        m_imguiDescriptorSet = ImGui_ImplVulkan_AddTexture(m_sampler, m_colorImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    void VulkanViewportRenderTarget::CreateSampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        if (vkCreateSampler(m_device, &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS)
            throw std::runtime_error("Failed to create viewport sampler");
    }

    uint32_t VulkanViewportRenderTarget::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
    {
        VkPhysicalDeviceMemoryProperties memoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memoryProperties);

        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            bool typeMatches = (typeFilter & (1 << i)) != 0;
            bool propertiesMatch = (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties;

            if (typeMatches && propertiesMatch)
                return i;
        }

        throw std::runtime_error("Failed to find suitable memory type");
    }
}