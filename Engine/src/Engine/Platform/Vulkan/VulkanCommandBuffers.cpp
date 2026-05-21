#include "VulkanCommandBuffers.h"

#include <stdexcept>
#include <array>
#include <Engine/Core/Log.h>

namespace Engine
{
    VulkanCommandBuffers::VulkanCommandBuffers(
        VkDevice device,
        VkCommandPool commandPool,
        VkRenderPass renderPass,
        VkPipeline pipeline,
        const std::vector<VkFramebuffer>& framebuffers,
        VkExtent2D extent
    )
        : m_device(device), m_commandPool(commandPool)
    {
        AllocateCommandBuffers(static_cast<uint32_t>(framebuffers.size()));
        RecordCommandBuffers(renderPass,pipeline, framebuffers, extent);
    }

    VulkanCommandBuffers::~VulkanCommandBuffers()
    {
        if (!m_commandBuffers.empty())
        {
            vkFreeCommandBuffers(
                m_device,
                m_commandPool,
                static_cast<uint32_t>(m_commandBuffers.size()),
                m_commandBuffers.data()
            );

            m_commandBuffers.clear();
        }
    }

    void VulkanCommandBuffers::AllocateCommandBuffers(uint32_t count)
    {
        m_commandBuffers.resize(count);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = count;

        if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate Vulkan command buffers");
        }
    }

    void VulkanCommandBuffers::RecordCommandBuffers(
        VkRenderPass renderPass, VkPipeline pipeline,
        const std::vector<VkFramebuffer>& framebuffers,
        VkExtent2D extent
    )
    {
        for (size_t i = 0; i < m_commandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to begin recording Vulkan command buffer");
            }

            VkClearValue clearColor{};
            clearColor.color = { { 1.0f, 0.0f, 0.0f, 1.0f } };

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = framebuffers[i];
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = extent;
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            
            ENGINE_INFO("Recording command buffer for framebuffer index: {}", static_cast<uint32_t>(i));
            vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
           // vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
            //vkCmdDraw(m_commandBuffers[i], 3, 1, 0, 0);
            vkCmdEndRenderPass(m_commandBuffers[i]);
        }
        ENGINE_INFO("Recording command buffers. Count: {}", static_cast<uint32_t>(m_commandBuffers.size()));
        ENGINE_INFO("Framebuffer count: {0}", static_cast<uint32_t>(framebuffers.size()));
        ENGINE_INFO("Command buffer extent: {0} x {1}", extent.width, extent.height);
        ENGINE_INFO("Pipeline handle: {0}", reinterpret_cast<uint64_t>(pipeline));
    }


    void VulkanCommandBuffers::RecordCommandBuffer(uint32_t imageIndex, VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent, VkPipeline pipeline)
    {
        VkCommandBuffer commandBuffer = m_commandBuffers[imageIndex];

        vkResetCommandBuffer(commandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
            throw std::runtime_error("Failed to begin recording Vulkan command buffer");

        VkClearValue clearColor{};
        clearColor.color = { { 1.0f, 0.0f, 0.0f, 1.0f } };

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = framebuffer;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = extent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
            throw std::runtime_error("Failed to record Vulkan command buffer");
    }
}