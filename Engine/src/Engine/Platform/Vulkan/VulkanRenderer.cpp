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
    }

    void VulkanRenderer::Shutdown()
    {
        if (!m_context)
            return;

        vkDeviceWaitIdle(m_context->GetDeviceHandle());

        m_viewportPipeline.reset();
        m_viewportRenderTarget.Shutdown();

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

    void VulkanRenderer::BeginViewportRenderPass()
    {
        VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

        VkClearValue clearColor{};
        clearColor.color = { { 0.02f, 0.08f, 0.92f, 1.0f } };

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_viewportRenderTarget.GetRenderPass();
        renderPassInfo.framebuffer = m_viewportRenderTarget.GetFramebuffer();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_viewportRenderTarget.GetExtent();
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanRenderer::RenderViewport()
    {


        VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

        m_gameRenderer.Render(commandBuffer, m_viewportRenderTarget);


        
    }


    void VulkanRenderer::EndViewportRenderPass()
    {
        vkCmdEndRenderPass(GetCurrentCommandBuffer());
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