#include "VulkanInstance.h"

#include <GLFW/glfw3.h>

#include <vector>
#include <stdexcept>

namespace Engine
{

    static const std::vector<const char*> s_validationLayers =
    {
        "VK_LAYER_KHRONOS_validation"
    };

    VulkanInstance::VulkanInstance()
    {
        CreateInstance();

        if (s_enableValidationLayers)
        {
            m_debugMessenger = std::make_unique<VulkanDebugMessenger>(m_instance);
        }
    }

    VulkanInstance::~VulkanInstance()
    {
        m_debugMessenger.reset();
        if (m_instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(m_instance, nullptr);
            m_instance = VK_NULL_HANDLE;
        }
    }

    void VulkanInstance::CreateInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "VoxelEvaEngine";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "EvaEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        uint32_t glfwExtensionCount = 0;
        std::vector<const char*> instanceExtensions = GetRequiredExtensions();

        

       

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
        createInfo.ppEnabledExtensionNames = instanceExtensions.data();
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;

        if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan instance");
        }

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

        if (s_enableValidationLayers)
        {
            if (!CheckValidationLayerSupport())
            {
                throw std::runtime_error("Validation layers requested, but not available");
            }

            createInfo.enabledLayerCount = static_cast<uint32_t>(s_validationLayers.size());
            createInfo.ppEnabledLayerNames = s_validationLayers.data();

            VulkanDebugMessenger::PopulateCreateInfo(debugCreateInfo);
            createInfo.pNext = &debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;
            createInfo.pNext = nullptr;
        }
    }

    std::vector<const char*> VulkanInstance::GetRequiredExtensions() const
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (s_enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool VulkanInstance::CheckValidationLayerSupport() const
    {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : s_validationLayers)
        {
            bool layerFound = false;

            for (const VkLayerProperties& layerProperties : availableLayers)
            {
                if (std::strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }
}