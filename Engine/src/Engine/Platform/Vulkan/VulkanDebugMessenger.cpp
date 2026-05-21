#include "VulkanDebugMessenger.h"

#include "Engine/Core/Log.h"

#include <stdexcept>

namespace Engine
{
    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData)
    {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            ENGINE_ERROR("Vulkan validation: {0}", callbackData->pMessage);
        }
        else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            ENGINE_WARN("Vulkan validation: {0}", callbackData->pMessage);
        }
        else
        {
            ENGINE_INFO("Vulkan validation: {0}", callbackData->pMessage);
        }

        return VK_FALSE;
    }

    static VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
        const VkAllocationCallbacks* allocator,
        VkDebugUtilsMessengerEXT* debugMessenger)
    {
        auto function = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

        if (function)
        {
            return function(instance, createInfo, allocator, debugMessenger);
        }

        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    static void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* allocator)
    {
        auto function = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

        if (function)
        {
            function(instance, debugMessenger, allocator);
        }
    }

    VulkanDebugMessenger::VulkanDebugMessenger(VkInstance instance)
        : m_instance(instance)
    {
        CreateDebugMessenger();
    }

    VulkanDebugMessenger::~VulkanDebugMessenger()
    {
        if (m_debugMessenger != VK_NULL_HANDLE)
        {
            DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
            m_debugMessenger = VK_NULL_HANDLE;
        }
    }

    void VulkanDebugMessenger::PopulateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        createInfo.pfnUserCallback = VulkanDebugCallback;
        createInfo.pUserData = nullptr;
    }

    void VulkanDebugMessenger::CreateDebugMessenger()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        PopulateCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan debug messenger");
        }

        ENGINE_INFO("Vulkan debug messenger created");
    }
}