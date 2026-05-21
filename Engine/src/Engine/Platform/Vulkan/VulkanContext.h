#pragma once

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

#include <Engine/Renderer/GraphicsContext.h>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <memory>
#include <vulkan/vulkan_core.h>
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffers.h"
#include "VulkanSyncObjects.h"
#include "VulkanPipeline.h"

namespace Engine
{
    

    class VulkanContext : public GraphicsContext
    {
    public:
        VulkanContext(GLFWwindow* window);
        ~VulkanContext() override;

        void Init() override;
        void Shutdown() override;

        void SwapBuffers() override;
        void OnFramebufferResized() override { m_framebufferResized = true; }
        bool BeginFrame() override;
        void EndFrame() override;
        void DrawFrame() override;

        VkInstance GetInstance() const { return m_instance->GetInstance(); }
        VkSurfaceKHR GetSurface() const { return m_surface; }

        VulkanDevice& GetDevice() { return *m_device; }
        VulkanSwapchain& GetSwapchain() { return *m_swapchain; }
        VkDevice GetDeviceHandle() const { return m_device->GetDevice(); }
        VkRenderPass GetRenderPassHandle() const { return m_renderPass->GetRenderPass(); }
        VkCommandBuffer GetCurrentCommandBuffer() const { return m_commandBuffers->GetCommandBuffer(m_currentImageIndex); }
        uint32_t GetCurrentImageIndex() const { return m_currentImageIndex; }
        uint32_t GetImageCount() const { return static_cast<uint32_t>(m_swapchain->GetImages().size()); }
        VkExtent2D GetSwapchainExtent() const { return m_swapchain->GetExtent(); }
        void SetFramebufferResized(bool resized) { m_framebufferResized = resized; }
        GLFWwindow* GetNativeWindow() const { return m_window; }
        VkInstance GetInstanceHandle() const { return m_instance->GetInstance(); }
        VkPhysicalDevice GetPhysicalDeviceHandle() const { return m_physicalDevice->GetHandle(); }
        VkQueue GetGraphicsQueue() const { return m_device->GetGraphicsQueue(); }
        uint32_t GetGraphicsQueueFamily() const { return m_device->GetGraphicsQueueFamily(); }
    private:
        void RecreateSwapchain();
        void CreateSurface();

    private:
        GLFWwindow* m_window = nullptr;

        std::unique_ptr<VulkanInstance> m_instance;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;

        std::unique_ptr<VulkanPhysicalDevice> m_physicalDevice;
        std::unique_ptr<VulkanDevice> m_device;
        std::unique_ptr<VulkanSwapchain> m_swapchain;

        std::unique_ptr<VulkanRenderPass> m_renderPass;
        std::unique_ptr<VulkanFramebuffer> m_framebuffer;

        std::unique_ptr<VulkanCommandPool> m_commandPool;
        std::unique_ptr<VulkanCommandBuffers> m_commandBuffers;

        std::unique_ptr<VulkanSyncObjects> m_syncObjects;

        std::unique_ptr<VulkanPipeline> m_pipeline;

        uint32_t m_currentFrame = 0;
        uint32_t m_currentImageIndex = 0;
        bool m_framebufferResized = false;
    };
}