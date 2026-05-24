
#include "VulkanViewportImage.h"
#include "VulkanContext.h"

#include <stdexcept>
#include <imgui_impl_vulkan.h>
namespace Engine
{
    VulkanViewportImage::~VulkanViewportImage()
    {
        Shutdown();
    }

    void VulkanViewportImage::Init(VulkanContext* context, VkExtent2D extent, VkFormat format)
    {
        m_context = context;
        m_extent = extent;
        m_format = format;

        CreateImage();
        CreateImageView();
        CreateSampler();
        CreateRenderPass();
        CreateFramebuffer();
    }



    void VulkanViewportImage::Shutdown()
    {
        if (!m_context)
            return;

        VkDevice device = m_context->GetDeviceHandle();

        vkDeviceWaitIdle(device);

        if (m_imguiDescriptorSet != VK_NULL_HANDLE)
        {
            ImGui_ImplVulkan_RemoveTexture(m_imguiDescriptorSet);
            m_imguiDescriptorSet = VK_NULL_HANDLE;
        }

        if (m_sampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(device, m_sampler, nullptr);
            m_sampler = VK_NULL_HANDLE;
        }

        if (m_imageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(device, m_imageView, nullptr);
            m_imageView = VK_NULL_HANDLE;
        }

        if (m_image != VK_NULL_HANDLE)
        {
            vkDestroyImage(device, m_image, nullptr);
            m_image = VK_NULL_HANDLE;
        }

        if (m_memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, m_memory, nullptr);
            m_memory = VK_NULL_HANDLE;
        }

        m_context = nullptr;
        m_extent = {};
    }
    void VulkanViewportImage::DestroyImGuiDescriptor()
    {
        if (m_imguiDescriptorSet == VK_NULL_HANDLE)
            return;

        ImGui_ImplVulkan_RemoveTexture(m_imguiDescriptorSet);
        m_imguiDescriptorSet = VK_NULL_HANDLE;
    }



    void VulkanViewportImage::Resize(VkExtent2D extent)
    {
        if (extent.width == 0 || extent.height == 0)
            return;

        if (extent.width == m_extent.width && extent.height == m_extent.height)
            return;

        VulkanContext* context = m_context;
        VkFormat format = m_format;

        Shutdown();
        Init(context, extent, format);
    }

    void VulkanViewportImage::CreateImage()
    {
        VkDevice device = m_context->GetDeviceHandle();

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = m_extent.width;
        imageInfo.extent.height = m_extent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = m_format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage =
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT |
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
            VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device, &imageInfo, nullptr, &m_image) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan viewport image");
        }

        VkMemoryRequirements memoryRequirements{};
        vkGetImageMemoryRequirements(device, m_image, &memoryRequirements);

        VkMemoryAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = memoryRequirements.size;
        allocateInfo.memoryTypeIndex = FindMemoryType(
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        if (vkAllocateMemory(device, &allocateInfo, nullptr, &m_memory) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate Vulkan viewport image memory");
        }

        vkBindImageMemory(device, m_image, m_memory, 0);
    }

    void VulkanViewportImage::CreateImageView()
    {
        VkDevice device = m_context->GetDeviceHandle();

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_format;

        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &viewInfo, nullptr, &m_imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan viewport image view");
        }
    }

    void VulkanViewportImage::CreateSampler()
    {
        VkDevice device = m_context->GetDeviceHandle();

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 1.0f;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 1.0f;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        if (vkCreateSampler(device, &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan viewport sampler");
        }
    }

    void VulkanViewportImage::CreateRenderPass()
    {
        VkDevice device = m_context->GetDeviceHandle();

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependencies[2]{};

        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 2;
        renderPassInfo.pDependencies = dependencies;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create viewport render pass");
        }
    }


    void VulkanViewportImage::CreateFramebuffer()
    {
        VkDevice device = m_context->GetDeviceHandle();

        VkImageView attachments[] =
        {
            m_imageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_extent.width;
        framebufferInfo.height = m_extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_framebuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create viewport framebuffer");
        }
    }


    void VulkanViewportImage::CreateImGuiDescriptor()
    {
        m_imguiDescriptorSet = ImGui_ImplVulkan_AddTexture(
            m_sampler,
            m_imageView,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
    }

   


    uint32_t VulkanViewportImage::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
    {
        VkPhysicalDeviceMemoryProperties memoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(
            m_context->GetPhysicalDeviceHandle(),
            &memoryProperties
        );

        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            bool hasType = (typeFilter & (1 << i)) != 0;
            bool hasProperties =
                (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties;

            if (hasType && hasProperties)
                return i;
        }

        throw std::runtime_error("Failed to find suitable Vulkan memory type");
    }
}