#include "VulkanShaderModule.h"

#include <fstream>
#include <stdexcept>

namespace Engine
{
    VulkanShaderModule::VulkanShaderModule(VkDevice device, const std::string& filepath)
        : m_device(device)
    {
        std::vector<char> code = ReadFile(filepath);
        CreateShaderModule(code);
    }

    VulkanShaderModule::~VulkanShaderModule()
    {
        if (m_shaderModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(m_device, m_shaderModule, nullptr);
            m_shaderModule = VK_NULL_HANDLE;
        }
    }

    std::vector<char> VulkanShaderModule::ReadFile(const std::string& filepath)
    {
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open shader file: " + filepath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    void VulkanShaderModule::CreateShaderModule(const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(m_device, &createInfo, nullptr, &m_shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan shader module");
        }
    }
}