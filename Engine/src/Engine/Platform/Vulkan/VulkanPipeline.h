#pragma once

#include <vulkan/vulkan.h>
#include <string>

namespace Engine
{
    struct VulkanPipelineSpecification;
    class VulkanPipeline
    {
    public:
        VulkanPipeline(VkDevice device, VulkanPipelineSpecification spec);
        ~VulkanPipeline();

        VulkanPipeline(const VulkanPipeline&) = delete;
        VulkanPipeline& operator=(const VulkanPipeline&) = delete;

        VkPipeline GetPipeline() const { return m_pipeline; }
        VkPipelineLayout GetPipelineLayout() const { return m_pipelineLayout; }

    private:
        void CreateGraphicsPipeline(VkExtent2D extent, VkRenderPass renderPass, const std::string& shaderPath);

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        VkPipeline m_pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    };
}