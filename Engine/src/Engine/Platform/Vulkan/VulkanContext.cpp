#include "VulkanContext.h"


#include <GLFW/glfw3.h>
#include <stdexcept>
#include "Engine/Platform/WindowsWindow.h"
#include <Engine/Core/Log.h>
#include <Engine/Core/Assert.h>
#include <limits>
#include <filesystem>
#include "VulkanPipelineSpecification.h"


namespace Engine
{
    VulkanContext::VulkanContext(GLFWwindow* window)
        : m_window(window)
    {
    }

    VulkanContext::~VulkanContext()
    {
        // Important destruction order:
        // swapchain before device
        // device before surface
        // surface before instance

        Shutdown();
    }

    static const char* VkResultToString(VkResult result)
    {
        switch (result)
        {
        case VK_SUCCESS: return "VK_SUCCESS";
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_EVENT_SET: return "VK_EVENT_SET";
        case VK_EVENT_RESET: return "VK_EVENT_RESET";
        case VK_INCOMPLETE: return "VK_INCOMPLETE";

        case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";

        case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";

        default: return "Unknown VkResult";
        }
    }

    void VulkanContext::Init()
    {
        m_instance = std::make_unique<VulkanInstance>();

        CreateSurface();
        ENGINE_INFO("Vulkan surface handle: {0}", reinterpret_cast<uint64_t>(m_surface));
        ENGINE_INFO("VulkanContext Init with GLFW window: {0}", reinterpret_cast<uint64_t>(m_window));
        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
            {
                VulkanContext* context = static_cast<VulkanContext*>(glfwGetWindowUserPointer(window));
                context->SetFramebufferResized(true);
            });

        m_physicalDevice = std::make_unique<VulkanPhysicalDevice>(
            m_instance->GetInstance(),
            m_surface
        );

        VkPhysicalDevice physicalDevice = m_physicalDevice->GetHandle();

        int windowWidth = 0;
        int windowHeight = 0;
        glfwGetFramebufferSize(m_window, &windowWidth, &windowHeight);

        if (windowWidth == 0 || windowHeight == 0)
        {
            return;
        }

        m_device = std::make_unique<VulkanDevice>(
            physicalDevice,
            m_surface
        );

        m_swapchain = std::make_unique<VulkanSwapchain>(
            physicalDevice,
            m_device->GetDevice(),
            m_surface,
            static_cast<uint32_t>(windowWidth),
            static_cast<uint32_t>(windowHeight),
            m_device->GetGraphicsQueueFamily(),
            m_device->GetPresentQueueFamily()
        );

        m_renderPass = std::make_unique<VulkanRenderPass>(
            m_device->GetDevice(),
            m_swapchain->GetImageFormat()
        );

        VulkanPipelineSpecification spec{};
        spec.Name = "Triangle";
        spec.ShaderPath = (std::filesystem::path(VE_SHADER_SOURCE_DIR) / "triangle.glsl").string();
        spec.RenderPass = m_renderPass->GetRenderPass();
        spec.Extent = m_swapchain->GetExtent();

        m_pipeline = std::make_unique<VulkanPipeline>(m_device->GetDevice(), spec);
        

        m_framebuffer = std::make_unique<VulkanFramebuffer>(
            m_device->GetDevice(),
            m_renderPass->GetRenderPass(),
            m_swapchain->GetImageViews(),
            m_swapchain->GetExtent()
        );

        m_commandPool = std::make_unique<VulkanCommandPool>(
            m_device->GetDevice(),
            m_device->GetGraphicsQueueFamily()
        );

        m_commandBuffers = std::make_unique<VulkanCommandBuffers>(
            m_device->GetDevice(),
            m_commandPool->GetCommandPool(),
            m_renderPass->GetRenderPass(),
            m_pipeline->GetPipeline(),
            m_framebuffer->GetFramebuffers(),
            m_swapchain->GetExtent()
        );

        m_syncObjects = std::make_unique<VulkanSyncObjects>(
            m_device->GetDevice()
        );
    }

    void VulkanContext::Shutdown()
    {
        if (m_device)
        {
            vkDeviceWaitIdle(m_device->GetDevice());
        }

        m_syncObjects.reset();

        m_commandBuffers.reset();

        m_pipeline.reset();
        m_commandPool.reset();

        m_framebuffer.reset();
        m_renderPass.reset();

        m_swapchain.reset();
        m_device.reset();
        m_physicalDevice.reset();

        if (m_surface != VK_NULL_HANDLE && m_instance)
        {
            vkDestroySurfaceKHR(m_instance->GetInstance(), m_surface, nullptr);
            m_surface = VK_NULL_HANDLE;
        }

        m_instance.reset();
    }

    bool VulkanContext::BeginFrame()
    {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_window, &width, &height);

        if (width == 0 || height == 0)
        {
            return false;
        }
      


        VkDevice device = m_device->GetDevice();
        VkFence inFlightFence = m_syncObjects->GetInFlightFence(m_currentFrame);

        vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

        VkSemaphore imageAvailableSemaphore = m_syncObjects->GetImageAvailableSemaphore(m_currentFrame);

        VkResult result = vkAcquireNextImageKHR(device, m_swapchain->GetSwapchain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &m_currentImageIndex);
        if (result == VK_NOT_READY)
        {
            ENGINE_WARN("Swapchain image not ready");
            return false;
        }
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapchain();
            return false;
        }


        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            ENGINE_ERROR("Failed to acquire Vulkan swapchain image. Result: {0} ({1})", VkResultToString(result), static_cast<int32_t>(result));
            return false;
        }

        vkResetFences(device, 1, &inFlightFence);
        return true;
    }

    void VulkanContext::EndFrame()
    {
        VkSemaphore waitSemaphores[] =
        {
            m_syncObjects->GetImageAvailableSemaphore(m_currentFrame)
        };

        VkPipelineStageFlags waitStages[] =
        {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        VkCommandBuffer commandBuffer =
            m_commandBuffers->GetCommandBuffer(m_currentImageIndex);

        VkSemaphore signalSemaphores[] =
        {
            m_syncObjects->GetRenderFinishedSemaphore(m_currentFrame)
        };

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VkResult result = vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, m_syncObjects->GetInFlightFence(m_currentFrame));
        if (result != VK_SUCCESS)
        {
            ENGINE_ERROR("vkQueueSubmit failed: {0}", static_cast<int32_t>(result));
            return;
        }
    }

    void VulkanContext::DrawFrame()
    {
        const std::vector<VkFramebuffer>& framebuffers = m_framebuffer->GetFramebuffers();

        m_commandBuffers->RecordCommandBuffer(
            m_currentImageIndex,
            m_renderPass->GetRenderPass(),
            framebuffers[m_currentImageIndex],
            m_swapchain->GetExtent(),
            m_pipeline->GetPipeline()
        );
    }

    void VulkanContext::SwapBuffers()
    {
        VkSemaphore renderFinishedSemaphore = m_syncObjects->GetRenderFinishedSemaphore(m_currentFrame);

        VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
        VkSwapchainKHR swapchains[] = { m_swapchain->GetSwapchain() };

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &m_currentImageIndex;

        VkResult result = vkQueuePresentKHR(m_device->GetPresentQueue(), &presentInfo);


        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
        {
            RecreateSwapchain();
        }
        else if (result != VK_SUCCESS)
        {
            ENGINE_ERROR("Failed to present Vulkan swapchain image. Result: {0} ({1})", VkResultToString(result), static_cast<int32_t>(result));
            throw std::runtime_error("Failed to present Vulkan swapchain image");
        }

        m_currentFrame = (m_currentFrame + 1) % VulkanSyncObjects::MaxFramesInFlight;
    }

    void VulkanContext::RecreateSwapchain()
    {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_window, &width, &height);

        if (width == 0 || height == 0)
        {
            return;
        }

        vkDeviceWaitIdle(m_device->GetDevice());

        m_commandBuffers.reset();
        m_framebuffer.reset();
        m_renderPass.reset();
        m_swapchain.reset();
        m_pipeline.reset();
        VkPhysicalDevice physicalDevice = m_physicalDevice->GetHandle();

        m_swapchain = std::make_unique<VulkanSwapchain>(
            physicalDevice,
            m_device->GetDevice(),
            m_surface,
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height),
            m_device->GetGraphicsQueueFamily(),
            m_device->GetPresentQueueFamily()
        );

        m_renderPass = std::make_unique<VulkanRenderPass>(m_device->GetDevice(), m_swapchain->GetImageFormat());

        VulkanPipelineSpecification spec{};
        spec.Name = "Triangle";
        spec.ShaderPath = (std::filesystem::path(VE_SHADER_SOURCE_DIR) / "triangle.glsl").string();
        spec.RenderPass = m_renderPass->GetRenderPass();
        spec.Extent = m_swapchain->GetExtent();

        m_pipeline = std::make_unique<VulkanPipeline>(m_device->GetDevice(), spec);


        m_framebuffer = std::make_unique<VulkanFramebuffer>(
            m_device->GetDevice(),
            m_renderPass->GetRenderPass(),
            m_swapchain->GetImageViews(),
            m_swapchain->GetExtent()
        );

        m_commandBuffers = std::make_unique<VulkanCommandBuffers>(
            m_device->GetDevice(),
            m_commandPool->GetCommandPool(),
            m_renderPass->GetRenderPass(),
            m_pipeline->GetPipeline(),
            m_framebuffer->GetFramebuffers(),
            m_swapchain->GetExtent()
        );

        m_framebufferResized = false;

        ENGINE_INFO("Vulkan swapchain recreated");
    }

    void VulkanContext::CreateSurface()
    {
        if (glfwCreateWindowSurface(
            m_instance->GetInstance(),
            m_window,
            nullptr,
            &m_surface
        ) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan window surface");
        }
    }
}