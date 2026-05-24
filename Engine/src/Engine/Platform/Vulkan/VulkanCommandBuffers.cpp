#include "VulkanCommandBuffers.h"

#include <stdexcept>
#include <array>
#include <Engine/Core/Log.h>

namespace Engine
{
    VulkanCommandBuffers::VulkanCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t count)
        : m_device(device), m_commandPool(commandPool)
    {
        AllocateCommandBuffers(count);
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



    void VulkanCommandBuffers::RecordCommandBuffer(uint32_t imageIndex, VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent, VkPipeline pipeline)
    {
        VkCommandBuffer commandBuffer = m_commandBuffers[imageIndex];


       
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);
      
    }
}