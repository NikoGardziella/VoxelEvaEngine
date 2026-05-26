#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Engine::VulkanUtils
{
    uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkCommandBuffer BeginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);

    void EndSingleTimeCommands(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkCommandBuffer commandBuffer);
}