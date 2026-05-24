#include "EditorImGuiLayer.h"

#include <Engine/Core/Log.h>
#include <Engine/Platform/Vulkan/VulkanContext.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <GLFW/glfw3.h>
#include <Engine/Core/Assert.h>

namespace Editor
{
    static void CheckVkResult(VkResult result)
    {
        if (result == VK_SUCCESS)
            return;

        ENGINE_ERROR("ImGui Vulkan error: {0}", static_cast<int32_t>(result));
    }

    void EditorImGuiLayer::OnAttach(Engine::VulkanContext* context, Engine::VulkanRenderer* renderer)
    {
        m_context = context;
        m_renderer = renderer;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        CreateDescriptorPool();

        ImGui_ImplGlfw_InitForVulkan(m_context->GetNativeWindow(), true);

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.ApiVersion = VK_API_VERSION_1_3;
        initInfo.Instance = m_context->GetInstanceHandle();
        initInfo.PhysicalDevice = m_context->GetPhysicalDeviceHandle();
        initInfo.Device = m_context->GetDeviceHandle();
        initInfo.QueueFamily = m_context->GetGraphicsQueueFamily();
        initInfo.Queue = m_context->GetGraphicsQueue();
        initInfo.DescriptorPool = m_descriptorPool;
        initInfo.DescriptorPoolSize = 0;
        initInfo.MinImageCount = 2;
        initInfo.ImageCount = m_renderer->GetImageCount();
        initInfo.PipelineCache = VK_NULL_HANDLE;

        initInfo.PipelineInfoMain.RenderPass = m_renderer->GetRenderPass();
        initInfo.PipelineInfoMain.Subpass = 0;
        initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        initInfo.PipelineInfoForViewports.RenderPass = m_renderer->GetRenderPass();
        initInfo.PipelineInfoForViewports.Subpass = 0;
        initInfo.PipelineInfoForViewports.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        initInfo.UseDynamicRendering = false;
        initInfo.Allocator = nullptr;
        initInfo.CheckVkResultFn = CheckVkResult;
        initInfo.MinAllocationSize = 1024 * 1024;

        EE_CORE_ASSERT(initInfo.PipelineInfoMain.RenderPass != VK_NULL_HANDLE, "ImGui render pass is null");
        EE_CORE_ASSERT(initInfo.ImageCount >= initInfo.MinImageCount, "Invalid ImGui image count");

        ImGui_ImplVulkan_Init(&initInfo);
        if (renderer)
        {
            m_renderer->GetViewportRenderTarget().CreateImGuiDescriptor();

        }
        ENGINE_INFO("Editor ImGui layer attached");
    }

    void EditorImGuiLayer::OnDetach()
    {
        VkDevice device = m_context->GetDeviceHandle();

        vkDeviceWaitIdle(device);

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        if (m_descriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);
            m_descriptorPool = VK_NULL_HANDLE;
        }

        m_context = nullptr;
    }

    void EditorImGuiLayer::Begin()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void EditorImGuiLayer::End(VkCommandBuffer commandBuffer)
    {
        ImGui::Render();

        ImDrawData* drawData = ImGui::GetDrawData();

        if (drawData && drawData->CmdListsCount > 0)
        {
            ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
        }
    }

    void EditorImGuiLayer::CreateDescriptorPool()
    {
        VkDescriptorPoolSize poolSizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = 1000 * static_cast<uint32_t>(std::size(poolSizes));
        poolInfo.poolSizeCount = static_cast<uint32_t>(std::size(poolSizes));
        poolInfo.pPoolSizes = poolSizes;

        if (vkCreateDescriptorPool(m_context->GetDeviceHandle(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create ImGui descriptor pool");
        }
    }
}