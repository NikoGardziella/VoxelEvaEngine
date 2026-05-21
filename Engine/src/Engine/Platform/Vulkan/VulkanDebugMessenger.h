#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
    class VulkanDebugMessenger
    {
    public:
        VulkanDebugMessenger(VkInstance instance);
        ~VulkanDebugMessenger();

        VulkanDebugMessenger(const VulkanDebugMessenger&) = delete;
        VulkanDebugMessenger& operator=(const VulkanDebugMessenger&) = delete;

        static void PopulateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    private:
        void CreateDebugMessenger();

    private:
        VkInstance m_instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    };
}