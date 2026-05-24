#include "VulkanMeshBuffer.h"

#include <cstring>
#include <stdexcept>

namespace Engine
{
    VulkanMeshBuffer::~VulkanMeshBuffer()
    {
        Shutdown();
    }

    void VulkanMeshBuffer::Init(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
    {
        m_device = device;
        m_physicalDevice = physicalDevice;
        m_commandPool = commandPool;
        m_graphicsQueue = graphicsQueue;
    }

    void VulkanMeshBuffer::Shutdown()
    {
        if (m_indexBuffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(m_device, m_indexBuffer, nullptr);
            m_indexBuffer = VK_NULL_HANDLE;
        }

        if (m_indexBufferMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(m_device, m_indexBufferMemory, nullptr);
            m_indexBufferMemory = VK_NULL_HANDLE;
        }

        if (m_vertexBuffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(m_device, m_vertexBuffer, nullptr);
            m_vertexBuffer = VK_NULL_HANDLE;
        }

        if (m_vertexBufferMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(m_device, m_vertexBufferMemory, nullptr);
            m_vertexBufferMemory = VK_NULL_HANDLE;
        }

        m_indexCount = 0;
    }

    void VulkanMeshBuffer::SetData(const Mesh& mesh)
    {
        Shutdown();

        if (!mesh.IsValid())
            return;

        CreateVertexBuffer(mesh.GetVertices());
        CreateIndexBuffer(mesh.GetIndices());

        m_indexCount = mesh.GetIndexCount();
    }

    void VulkanMeshBuffer::Bind(VkCommandBuffer commandBuffer) const
    {
        VkBuffer vertexBuffers[] = { m_vertexBuffer };
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanMeshBuffer::Draw(VkCommandBuffer commandBuffer) const
    {
        if (!IsValid())
            return;

        Bind(commandBuffer);
        vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
    }

    void VulkanMeshBuffer::CreateVertexBuffer(const std::vector<MeshVertex>& vertices)
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data = nullptr;
        vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
        std::memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(m_device, stagingBufferMemory);

        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);
        CopyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

        vkDestroyBuffer(m_device, stagingBuffer, nullptr);
        vkFreeMemory(m_device, stagingBufferMemory, nullptr);
    }

    void VulkanMeshBuffer::CreateIndexBuffer(const std::vector<uint32_t>& indices)
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data = nullptr;
        vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
        std::memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(m_device, stagingBufferMemory);

        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);
        CopyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

        vkDestroyBuffer(m_device, stagingBuffer, nullptr);
        vkFreeMemory(m_device, stagingBufferMemory, nullptr);
    }

    void VulkanMeshBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory)
    {
        if (m_device == VK_NULL_HANDLE)
            throw std::runtime_error("VulkanMeshBuffer device is null");

        if (size == 0)
            throw std::runtime_error("Cannot create Vulkan buffer with size 0");

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
            throw std::runtime_error("Failed to create Vulkan buffer");

        VkMemoryRequirements memoryRequirements{};
        vkGetBufferMemoryRequirements(m_device, buffer, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate Vulkan buffer memory");

        vkBindBufferMemory(m_device, buffer, memory, 0);
    }

    void VulkanMeshBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;

        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_graphicsQueue);

        vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
    }

    uint32_t VulkanMeshBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
    {
        VkPhysicalDeviceMemoryProperties memoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memoryProperties);

        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            bool typeMatches = (typeFilter & (1 << i)) != 0;
            bool propertiesMatch = (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties;

            if (typeMatches && propertiesMatch)
                return i;
        }

        throw std::runtime_error("Failed to find suitable Vulkan memory type");
    }
}