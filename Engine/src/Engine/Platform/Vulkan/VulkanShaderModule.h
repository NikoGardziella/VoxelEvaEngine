#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace Engine
{
    class VulkanShaderModule
    {
    public:
        VulkanShaderModule(VkDevice device, const std::string& filepath);
        ~VulkanShaderModule();

        VulkanShaderModule(const VulkanShaderModule&) = delete;
        VulkanShaderModule& operator=(const VulkanShaderModule&) = delete;

        VkShaderModule GetShaderModule() const { return m_shaderModule; }

    private:
        std::vector<char> ReadFile(const std::string& filepath);
        void CreateShaderModule(const std::vector<char>& code);

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        VkShaderModule m_shaderModule = VK_NULL_HANDLE;
    };
}