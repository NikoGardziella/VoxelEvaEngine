#pragma once

#include <vulkan/vulkan.h>
#include "VulkanDebugMessenger.h"
#include <memory>
#include <vector>

#ifdef VE_DEBUG
static constexpr bool s_enableValidationLayers = true;
#else
static constexpr bool s_enableValidationLayers = false;
#endif

namespace Engine
{
    class VulkanInstance
    {
    public:
        VulkanInstance();
        ~VulkanInstance();

        VulkanInstance(const VulkanInstance&) = delete;
        VulkanInstance& operator=(const VulkanInstance&) = delete;

        VkInstance GetInstance() const { return m_instance; }

    private:
        void CreateInstance();
        bool CheckValidationLayerSupport() const;
        std::vector<const char*> GetRequiredExtensions() const;

    private:
        VkInstance m_instance = VK_NULL_HANDLE;
        std::unique_ptr<VulkanDebugMessenger> m_debugMessenger;
    };
}