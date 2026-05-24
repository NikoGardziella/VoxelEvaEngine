#include "VulkanContext.h"


#include <GLFW/glfw3.h>
#include <stdexcept>
#include "Engine/Platform/WindowsWindow.h"
#include <Engine/Core/Log.h>
#include <Engine/Core/Assert.h>
#include <limits>
#include <filesystem>
#include "VulkanPipelineSpecification.h"
#include "VulkanDebugMessenger.cpp"


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


        if (s_enableValidationLayers)
        {
            m_debugMessenger = std::make_unique<VulkanDebugMessenger>(m_instance->GetInstance());
        }
        

       
    }

    void VulkanContext::Shutdown()
    {
        if (m_device)
        {
            vkDeviceWaitIdle(m_device->GetDevice());
        }

     
        m_device.reset();
        m_physicalDevice.reset();

        if (m_surface != VK_NULL_HANDLE && m_instance)
        {
            vkDestroySurfaceKHR(m_instance->GetInstance(), m_surface, nullptr);
            m_surface = VK_NULL_HANDLE;
        }

        m_debugMessenger.reset();
        m_instance.reset();

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

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanContext::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
    {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            ENGINE_ERROR("Vulkan validation: {}", callbackData->pMessage);
        }
        else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            ENGINE_WARN("Vulkan validation: {}", callbackData->pMessage);
        }
        else
        {
            ENGINE_TRACE("Vulkan validation: {}", callbackData->pMessage);
        }

        return VK_FALSE;
    }



    void VulkanContext::TestValidationLayer()
    {
        ENGINE_WARN("Testing Vulkan validation layer");

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = 1024;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkBuffer buffer = VK_NULL_HANDLE;

        if (vkCreateBuffer(m_device->GetDevice(), &bufferInfo, nullptr, &buffer) == VK_SUCCESS)
        {
            vkDestroyBuffer(m_device->GetDevice(), buffer, nullptr);
            vkDestroyBuffer(m_device->GetDevice(), buffer, nullptr);
        }
    }
}