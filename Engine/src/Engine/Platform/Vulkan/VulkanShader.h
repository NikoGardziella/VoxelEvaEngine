#pragma once

#include <vulkan/vulkan.h>

#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>

namespace Engine {

    class VulkanShader
    {
    public:
        VulkanShader(const std::string& filepath, VkDevice device);
        VulkanShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
        virtual ~VulkanShader();

        

		VkShaderModule GetVertexShaderModule() const { return m_VertexShaderModule; }
		VkShaderModule GetFragmentShaderModule() const { return m_FragmentShaderModule; }
        VkShaderModule GetComputeshaderModule() const { return m_ComputeShaderModule; }
    private:

        std::string ReadFile(const std::string& filepath);
        std::unordered_map<VkShaderStageFlagBits, std::string> PreProcess(const std::string& source, const std::string& filepath);

        std::string ExpandIncludes(const std::string& src, const std::filesystem::path& directory);

        VkShaderStageFlagBits ShaderTypeFromString(const std::string& type);

        void CompileOrGetVulkanBinaries(const std::unordered_map<VkShaderStageFlagBits, std::string>& shaderSources, const std::string& originalFilePath);

        void CreateShaderModule(const std::vector<uint32_t>& code, VkShaderModule* shaderModule);

    private:
        std::string m_Name;
        VkDevice m_device;
        VkPipelineShaderStageCreateInfo m_ShaderStages[2];
        VkShaderModule m_VertexShaderModule;
        VkShaderModule m_FragmentShaderModule;
        VkShaderModule m_ComputeShaderModule;

        std::unordered_map<std::string, VkDescriptorSetLayoutBinding> m_UniformLocations;

    };

}
