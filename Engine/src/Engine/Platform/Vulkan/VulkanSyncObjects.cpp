#include "VulkanSyncObjects.h"

#include <stdexcept>

namespace Engine
{
    VulkanSyncObjects::VulkanSyncObjects(VkDevice device)
        : m_device(device)
    {
        CreateSyncObjects();
    }

    VulkanSyncObjects::~VulkanSyncObjects()
    {
        for (uint32_t i = 0; i < MaxFramesInFlight; i++)
        {
            vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
            vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
        }
    }

    VkSemaphore VulkanSyncObjects::GetImageAvailableSemaphore(uint32_t frameIndex) const
    {
        return m_imageAvailableSemaphores[frameIndex];
    }

    VkSemaphore VulkanSyncObjects::GetRenderFinishedSemaphore(uint32_t frameIndex) const
    {
        return m_renderFinishedSemaphores[frameIndex];
    }

    VkFence VulkanSyncObjects::GetInFlightFence(uint32_t frameIndex) const
    {
        return m_inFlightFences[frameIndex];
    }

    void VulkanSyncObjects::CreateSyncObjects()
    {
        m_imageAvailableSemaphores.resize(MaxFramesInFlight);
        m_renderFinishedSemaphores.resize(MaxFramesInFlight);
        m_inFlightFences.resize(MaxFramesInFlight);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (uint32_t i = 0; i < MaxFramesInFlight; i++)
        {
            if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create Vulkan sync objects");
            }
        }
    }
}