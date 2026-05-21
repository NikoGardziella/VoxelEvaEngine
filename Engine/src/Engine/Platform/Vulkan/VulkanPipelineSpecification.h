#pragma once
#include <string>
#include <vulkan/vulkan.h>

namespace Engine {

struct VulkanPipelineSpecification
{
    std::string Name;
    std::string ShaderPath;

    VkRenderPass RenderPass = VK_NULL_HANDLE;
    VkExtent2D Extent{};

    VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPolygonMode PolygonMode = VK_POLYGON_MODE_FILL;
    VkCullModeFlags CullMode = VK_CULL_MODE_NONE;
    VkFrontFace FrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    bool DepthTest = false;
    bool DepthWrite = false;
};
}