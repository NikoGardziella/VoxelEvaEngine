#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Engine
{
    class VulkanSyncObjects
    {
    public:
        static constexpr uint32_t MaxFramesInFlight = 2;

    public:
        VulkanSyncObjects(VkDevice device);
        ~VulkanSyncObjects();

        VulkanSyncObjects(const VulkanSyncObjects&) = delete;
        VulkanSyncObjects& operator=(const VulkanSyncObjects&) = delete;

        VkSemaphore GetImageAvailableSemaphore(uint32_t frameIndex) const;
        VkSemaphore GetRenderFinishedSemaphore(uint32_t frameIndex) const;
        VkFence GetInFlightFence(uint32_t frameIndex) const;

    private:
        void CreateSyncObjects();

    private:
        VkDevice m_device = VK_NULL_HANDLE;

        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkFence> m_inFlightFences;
    };
}