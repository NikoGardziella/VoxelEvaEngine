#include "VulkanRenderer.h"

#include "VulkanContext.h"
#include "Engine/Core/Log.h"

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <filesystem>

#include "VulkanPipelineSpecification.h"
#include "Engine/Renderer/Mesh/MeshFactory.h"
#include <Engine/Renderer/Mesh/Mesh.h>
#include <Engine/Voxel/VoxelChunk.h>
#include <Engine/Voxel/VoxelMesher.h>
#include <Engine/Assets/AssetManager.h>
#include <Engine/Voxel/VoxelAsset.h>

#include "Engine/Scene/Scene.h"
#include <Engine/Scene/Components/Core/TransformComponent.h>
#include <Engine/Scene/Components/Rendering/VoxelRendererComponent.h>

#include "Engine/Scene/Entity.h"
#include "VulkanUtils/VoxelPickingUtils.h"
#include "VulkanUtils/VulkanUtils.h"

namespace Engine
{
    static const char* VkResultToString(VkResult result)
    {
        switch (result)
        {
        case VK_SUCCESS: return "VK_SUCCESS";
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
        default: return "Unknown VkResult";
        }
    }

    VulkanRenderer::VulkanRenderer(VulkanContext* context, GLFWwindow* window)
        : m_context(context), m_window(window)
    {
    }

    VulkanRenderer::~VulkanRenderer()
    {
        Shutdown();
    }

    void VulkanRenderer::Init()
    {
        m_commandPool = std::make_unique<VulkanCommandPool>(m_context->GetDeviceHandle(), m_context->GetGraphicsQueueFamily());

        CreateSwapchainResources();

        m_syncObjects = std::make_unique<VulkanSyncObjects>(m_context->GetDeviceHandle());

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_window, &width, &height);

        m_gameRenderer.Init(m_context->GetDeviceHandle());

        m_viewportRenderTarget.Init(m_context->GetDeviceHandle(), m_context->GetPhysicalDeviceHandle(), m_context->GetGraphicsQueueFamily(), m_swapchain->GetImageFormat(), static_cast<uint32_t>(width), static_cast<uint32_t>(height));

        CreateViewportPipeline();


        m_voxelMeshBuffer.Init(m_context->GetDeviceHandle(), m_context->GetPhysicalDeviceHandle(), m_commandPool->GetCommandPool(), m_context->GetGraphicsQueue());
        m_gameRenderer.SetMeshBuffer(&m_voxelMeshBuffer);

        /*
        Engine::AssetManager assetManager;

        Engine::AssetHandle handle = assetManager.ImportAsset(VE_ENGINE_ASSET_DIR "/voxels/Zeds-0-Zed_1.vox", Engine::AssetType::Voxel);

        std::shared_ptr<Engine::Asset> asset = assetManager.GetAsset(handle);

        std::shared_ptr<Engine::VoxelAsset> voxelAsset =
            std::dynamic_pointer_cast<Engine::VoxelAsset>(asset);

        if (voxelAsset)
        {
            Engine::Mesh mesh = Engine::VoxelMesher::BuildMesh(voxelAsset->GetChunk());

            ENGINE_INFO("Voxel mesh vertices: {}", mesh.GetVertexCount());

            m_cubeMeshBuffer.SetData(mesh);
        }
        m_gameRenderer.SetMeshBuffer(&m_cubeMeshBuffer);
        */


        m_sceneRenderer.Init(m_context->GetDeviceHandle(), m_context->GetPhysicalDeviceHandle(), m_commandPool->GetCommandPool(), m_context->GetGraphicsQueue());
        //m_sceneRenderer.SetPipeline(m_viewportPipeline.get());

        VkFormat pickingFormat = VK_FORMAT_R32_UINT;
        m_pickingRenderTarget.Init(m_context->GetDeviceHandle(), m_context->GetPhysicalDeviceHandle(), pickingFormat, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

       

        CreatePickingPipeline();
    }

    void VulkanRenderer::CreateViewportPipeline()
    {
        VulkanPipelineSpecification viewportSpec{};
        viewportSpec.Name = "ViewportTriangle";
        viewportSpec.ShaderPath = (std::filesystem::path(VE_SHADER_SOURCE_DIR) / "triangle.glsl").string();
        viewportSpec.RenderPass = m_viewportRenderTarget.GetRenderPass();
        viewportSpec.Extent = m_viewportRenderTarget.GetExtent();

        m_viewportPipeline = std::make_unique<VulkanPipeline>(m_context->GetDeviceHandle(), viewportSpec);

        m_gameRenderer.SetPipeline(m_viewportPipeline.get());
        m_sceneRenderer.SetPipeline(m_viewportPipeline.get());
    }


    void VulkanRenderer::CreatePickingPipeline()
    {
        VulkanPipelineSpecification spec{};
        spec.Name = "VoxelPicking";
        spec.ShaderPath = (std::filesystem::path(VE_SHADER_SOURCE_DIR) / "VoxelPicking.glsl").string();
        spec.RenderPass = m_pickingRenderTarget.GetRenderPass();
        spec.Extent = m_pickingRenderTarget.GetExtent();

        m_pickingPipeline = std::make_unique<VulkanPipeline>(
            m_context->GetDeviceHandle(),
            spec
        );
        m_sceneRenderer.SetPickingPipeline(m_pickingPipeline.get());

    }

    void VulkanRenderer::SetViewProjection(const glm::mat4& viewProjection)
    {
        m_gameRenderer.SetViewProjection(viewProjection);
        m_sceneRenderer.SetViewProjection(viewProjection);
    }

    uint32_t VulkanRenderer::ReadPickingPixel(uint32_t x, uint32_t y)
    {
        VkDevice device = m_context->GetDeviceHandle();
        VkPhysicalDevice physicalDevice = m_context->GetPhysicalDeviceHandle();
        VkQueue graphicsQueue = m_context->GetGraphicsQueue();

        VkExtent2D extent = m_pickingRenderTarget.GetExtent();

        if (x >= extent.width || y >= extent.height)
            return 0;

        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingMemory = VK_NULL_HANDLE;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(uint32_t);
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS)
            return 0;

        VkMemoryRequirements memoryRequirements{};
        vkGetBufferMemoryRequirements(device, stagingBuffer, &memoryRequirements);

        VkMemoryAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = memoryRequirements.size;
        allocateInfo.memoryTypeIndex = Engine::VulkanUtils::FindMemoryType(
            physicalDevice,
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        if (vkAllocateMemory(device, &allocateInfo, nullptr, &stagingMemory) != VK_SUCCESS)
        {
            vkDestroyBuffer(device, stagingBuffer, nullptr);
            return 0;
        }

        vkBindBufferMemory(device, stagingBuffer, stagingMemory, 0);

        VkCommandBuffer commandBuffer = Engine::VulkanUtils::BeginSingleTimeCommands(
            device,
            m_commandPool->GetCommandPool()
        );

        VkBufferImageCopy copyRegion{};
        copyRegion.bufferOffset = 0;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;
        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageOffset = { static_cast<int32_t>(x), static_cast<int32_t>(y), 0 };
        copyRegion.imageExtent = { 1, 1, 1 };

        vkCmdCopyImageToBuffer(
            commandBuffer,
            m_pickingRenderTarget.GetImage(),
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            stagingBuffer,
            1,
            &copyRegion
        );

        Engine::VulkanUtils::EndSingleTimeCommands(
            device,
            graphicsQueue,
            m_commandPool->GetCommandPool(),
            commandBuffer
        );

        uint32_t pickedID = 0;

        void* data = nullptr;
        vkMapMemory(device, stagingMemory, 0, sizeof(uint32_t), 0, &data);
        std::memcpy(&pickedID, data, sizeof(uint32_t));
        vkUnmapMemory(device, stagingMemory);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingMemory, nullptr);

        return pickedID;
    }

    Engine::ViewportPickResult VulkanRenderer::ReadViewportPick(uint32_t x, uint32_t y)
    {
        Engine::ViewportPickResult result{};

        VkExtent2D extent = m_pickingRenderTarget.GetExtent();

        if (x >= extent.width || y >= extent.height)
            return result;

        uint32_t encodedID = ReadPickingPixel(x, y);

        result.RawID = encodedID;

        if (encodedID == 0)
            return result;

        uint32_t chunkIndex = 0;
        glm::ivec3 localVoxel{};

        if (!VoxelPickingUtils::DecodeVoxelPickID(encodedID, chunkIndex, localVoxel))
            return result;

        result.Hit = true;
        result.ChunkIndex = chunkIndex;
        result.LocalVoxel = localVoxel;
        result.LocalVoxelIndex =
            static_cast<uint32_t>(
                localVoxel.x +
                localVoxel.y * Engine::VoxelChunk::SizeX +
                localVoxel.z * Engine::VoxelChunk::SizeX * Engine::VoxelChunk::SizeY
                );

        result.WorldVoxel = localVoxel; // for now, until you add chunk/world origin

        return result;
    }

    void VulkanRenderer::Shutdown()
    {
        if (!m_context)
            return;

        vkDeviceWaitIdle(m_context->GetDeviceHandle());

        m_viewportPipeline.reset();
        m_viewportRenderTarget.Shutdown();
        m_pickingRenderTarget.Shutdown();
        m_syncObjects.reset();
        DestroySwapchainResources();
        m_commandPool.reset();
        m_cubeMeshBuffer.Shutdown();
        m_context = nullptr;
        m_window = nullptr;
    }

    bool VulkanRenderer::BeginFrame()
    {
        m_frameStarted = false;

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_window, &width, &height);

        if (width == 0 || height == 0)
            return false;

        VkDevice device = m_context->GetDeviceHandle();
        VkFence inFlightFence = m_syncObjects->GetInFlightFence(m_currentFrame);

        vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

        VkSemaphore imageAvailableSemaphore = m_syncObjects->GetImageAvailableSemaphore(m_currentFrame);
        VkResult result = vkAcquireNextImageKHR(device, m_swapchain->GetSwapchain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &m_currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapchain();
            return false;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            ENGINE_ERROR("Failed to acquire Vulkan swapchain image. Result: {0} ({1})", VkResultToString(result), static_cast<int32_t>(result));
            return false;
        }

        vkResetFences(device, 1, &inFlightFence);


        VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

        vkResetCommandBuffer(commandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin Vulkan command buffer");
        }

        m_frameStarted = true;

        return true;

    }

    void VulkanRenderer::BeginRenderPass()
    {
        VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

        VkClearValue clearColor{};
        clearColor.color = { { 0.92f, 0.02f, 0.02f, 1.0f } };

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass->GetRenderPass();
        renderPassInfo.framebuffer = m_framebuffer->GetFramebuffers()[m_currentImageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_swapchain->GetExtent();
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

   

    void VulkanRenderer::RenderScenePicking(Engine::Scene& scene)
    {
        VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

        m_sceneRenderer.RenderScenePicking(commandBuffer, m_pickingRenderTarget,  scene, m_assetManager);
    
    }

    void VulkanRenderer::EndPickingRenderPass()
    {
        VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

        vkCmdEndRenderPass(commandBuffer);
    }

    void VulkanRenderer::EndRenderPass()
    {
        vkCmdEndRenderPass(GetCurrentCommandBuffer());
    }

    void VulkanRenderer::EndFrame()
    {
        if (!m_frameStarted)
            return;


        VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to end Vulkan command buffer");
        }

        VkSemaphore waitSemaphores[] = { m_syncObjects->GetImageAvailableSemaphore(m_currentFrame) };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSemaphore signalSemaphores[] = { m_syncObjects->GetRenderFinishedSemaphore(m_currentFrame) };

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VkFence inFlightFence = m_syncObjects->GetInFlightFence(m_currentFrame);
        VkResult result = vkQueueSubmit(m_context->GetGraphicsQueue(), 1, &submitInfo, inFlightFence);

        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to submit Vulkan command buffer");
    }

    void VulkanRenderer::Present()
    {
        if (!m_frameStarted)
            return;

        VkSemaphore waitSemaphores[] = { m_syncObjects->GetRenderFinishedSemaphore(m_currentFrame) };
        VkSwapchainKHR swapchains[] = { m_swapchain->GetSwapchain() };

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = waitSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &m_currentImageIndex;

        VkResult result = vkQueuePresentKHR(m_context->GetPresentQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
        {
            RecreateSwapchain();
        }
        else if (result != VK_SUCCESS)
        {
            ENGINE_ERROR("Failed to present Vulkan swapchain image. Result: {0} ({1})", VkResultToString(result), static_cast<int32_t>(result));
            throw std::runtime_error("Failed to present Vulkan swapchain image");
        }

        m_currentFrame = (m_currentFrame + 1) % VulkanSyncObjects::MaxFramesInFlight;
        m_frameStarted = false;
    }


    void VulkanRenderer::RenderScene(Engine::Scene& scene)
    {
        VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();
        m_sceneRenderer.RenderScene(commandBuffer, m_viewportRenderTarget, scene, m_assetManager);
    }



    void VulkanRenderer::RequestViewportResize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;

       
        VkExtent2D currentExtent = m_viewportRenderTarget.GetExtent();

        if (currentExtent.width == width && currentExtent.height == height)
            return;

        m_pendingViewportExtent = { width, height };
        m_viewportResizePending = true;
    }

    
    void VulkanRenderer::ApplyPendingViewportResize()
    {
        if (!m_viewportResizePending)
            return;

        m_viewportResizePending = false;

        vkDeviceWaitIdle(m_context->GetDeviceHandle());

        m_viewportRenderTarget.Resize(m_pendingViewportExtent.width, m_pendingViewportExtent.height);

        CreateViewportPipeline();

        m_viewportRenderTarget.CreateImGuiDescriptor();
    }



    void VulkanRenderer::LoadVoxelAsset(const std::filesystem::path& path)
    {
        Engine::AssetHandle handle = m_assetManager.ImportAsset(path, Engine::AssetType::Voxel);
        std::shared_ptr<Engine::Asset> asset = m_assetManager.GetAsset(handle);
        std::shared_ptr<Engine::VoxelAsset> voxelAsset = std::dynamic_pointer_cast<Engine::VoxelAsset>(asset);

        if (!voxelAsset)
        {
            ENGINE_ERROR("Failed to load voxel asset: {}", path.string());
            return;
        }

        Engine::Mesh mesh = Engine::VoxelMesher::BuildMesh(voxelAsset->GetChunk());

        if (!mesh.IsValid())
        {
            ENGINE_ERROR("Loaded voxel asset produced empty mesh: {}", path.string());
            return;
        }

        m_voxelMeshBuffer.SetData(mesh);
        m_gameRenderer.SetMeshBuffer(&m_voxelMeshBuffer);

        ENGINE_INFO("Loaded voxel asset mesh: {} vertices, {} indices", mesh.GetVertexCount(), mesh.GetIndexCount());
    }

    VkCommandBuffer VulkanRenderer::GetCurrentCommandBuffer() const
    {
        return m_commandBuffers->GetCommandBuffer(m_currentImageIndex);
    }

    uint32_t VulkanRenderer::GetImageCount() const
    {
        return static_cast<uint32_t>(m_swapchain->GetImages().size());
    }

    ImTextureID VulkanRenderer::GetViewportTextureID() const
    {
        return m_viewportRenderTarget.GetImGuiTextureID();
    }
    
    void VulkanRenderer::CreateSwapchainResources()
    {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_window, &width, &height);

        m_swapchain = std::make_unique<VulkanSwapchain>(
            m_context->GetPhysicalDeviceHandle(),
            m_context->GetDeviceHandle(),
            m_context->GetSurfaceHandle(),
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height),
            m_context->GetGraphicsQueueFamily(),
            m_context->GetPresentQueueFamily()
        );

        m_renderPass = std::make_unique<VulkanRenderPass>(m_context->GetDeviceHandle(), m_swapchain->GetImageFormat());

        VulkanPipelineSpecification spec{};
        spec.Name = "Triangle";
        spec.ShaderPath = (std::filesystem::path(VE_SHADER_SOURCE_DIR) / "triangle.glsl").string();
        spec.RenderPass = m_renderPass->GetRenderPass();
        spec.Extent = m_swapchain->GetExtent();

        m_pipeline = std::make_unique<VulkanPipeline>(m_context->GetDeviceHandle(), spec);

        m_framebuffer = std::make_unique<VulkanFramebuffer>(
            m_context->GetDeviceHandle(),
            m_renderPass->GetRenderPass(),
            m_swapchain->GetImageViews(),
            m_swapchain->GetExtent()
        );

        m_commandBuffers = std::make_unique<VulkanCommandBuffers>(
            m_context->GetDeviceHandle(),
            m_commandPool->GetCommandPool(),
            static_cast<uint32_t>(m_swapchain->GetImages().size())
        );

     

     


    }


    void VulkanRenderer::DestroySwapchainResources()
    {
        m_commandBuffers.reset();
        m_framebuffer.reset();
        m_pipeline.reset();
        m_renderPass.reset();
        m_swapchain.reset();
    }

    void VulkanRenderer::RecreateSwapchain()
    {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_window, &width, &height);

        if (width == 0 || height == 0)
            return;

        vkDeviceWaitIdle(m_context->GetDeviceHandle());

        DestroySwapchainResources();
        CreateSwapchainResources();

        CreateViewportPipeline();

        m_framebufferResized = false;

        ENGINE_INFO("Vulkan swapchain recreated");
    }


    
}