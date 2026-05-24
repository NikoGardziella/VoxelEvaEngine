#pragma once

#include "Engine/Renderer/Mesh/Mesh.h"

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Engine
{
    class VulkanMeshBuffer
    {
    public:
        VulkanMeshBuffer() = default;
        ~VulkanMeshBuffer();

        VulkanMeshBuffer(const VulkanMeshBuffer&) = delete;
        VulkanMeshBuffer& operator=(const VulkanMeshBuffer&) = delete;
        VulkanMeshBuffer(VulkanMeshBuffer&&) = delete;
        VulkanMeshBuffer& operator=(VulkanMeshBuffer&&) = delete;

        void Init(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
        void Shutdown();

        void SetData(const Mesh& mesh);
        void Bind(VkCommandBuffer commandBuffer) const;
        void Draw(VkCommandBuffer commandBuffer) const;

        bool IsValid() const { return m_vertexBuffer != VK_NULL_HANDLE && m_indexBuffer != VK_NULL_HANDLE && m_indexCount > 0; }

    private:
        void CreateVertexBuffer(const std::vector<MeshVertex>& vertices);
        void CreateIndexBuffer(const std::vector<uint32_t>& indices);
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        VkCommandPool m_commandPool = VK_NULL_HANDLE;
        VkQueue m_graphicsQueue = VK_NULL_HANDLE;

        VkBuffer m_vertexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_vertexBufferMemory = VK_NULL_HANDLE;

        VkBuffer m_indexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_indexBufferMemory = VK_NULL_HANDLE;

        uint32_t m_indexCount = 0;
    };
}