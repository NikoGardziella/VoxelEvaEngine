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
#include "VulkanDebugMessenger.h"

namespace Engine
{
    

    class VulkanContext : public GraphicsContext
    {
    public:
        VulkanContext(GLFWwindow* window);
        ~VulkanContext() override;

        void Init() override;
        void Shutdown() override;

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData);


        void TestValidationLayer();


        VkInstance GetInstance() const { return m_instance->GetInstance(); }
        VkSurfaceKHR GetSurface() const { return m_surface; }

        VulkanDevice& GetDevice() { return *m_device; }
        VkDevice GetDeviceHandle() const { return m_device->GetDevice(); }
        uint32_t GetCurrentImageIndex() const { return m_currentImageIndex; }
        void SetFramebufferResized(bool resized) { m_framebufferResized = resized; }
        GLFWwindow* GetNativeWindow() const { return m_window; }
        VkInstance GetInstanceHandle() const { return m_instance->GetInstance(); }
        VkPhysicalDevice GetPhysicalDeviceHandle() const { return m_physicalDevice->GetHandle(); }
        VkQueue GetGraphicsQueue() const { return m_device->GetGraphicsQueue(); }
        uint32_t GetGraphicsQueueFamily() const { return m_device->GetGraphicsQueueFamily(); }
        uint32_t GetPresentQueueFamily() const { return m_device->GetPresentQueueFamily(); }
        VkSurfaceKHR GetSurfaceHandle() const { return m_surface; }
        VkQueue GetPresentQueue() const { return m_device->GetPresentQueue(); }

    private:
        void CreateSurface();

    private:
        GLFWwindow* m_window = nullptr;

        std::unique_ptr<VulkanInstance> m_instance;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;

        std::unique_ptr<VulkanPhysicalDevice> m_physicalDevice;
        std::unique_ptr<VulkanDevice> m_device;



        std::unique_ptr<VulkanDebugMessenger> m_debugMessenger;

        uint32_t m_currentFrame = 0;
        uint32_t m_currentImageIndex = 0;
        bool m_framebufferResized = false;
    };
}