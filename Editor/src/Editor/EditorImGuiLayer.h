#include "Engine/Platform/Vulkan/VulkanContext.h"
#include <Engine/Platform/Vulkan/VulkanRenderer.h>


namespace Editor
{
    class EditorImGuiLayer
    {
    public:
        void OnAttach(Engine::VulkanContext* context, Engine::VulkanRenderer* renderer);
        void OnDetach();

        void Begin();
        void End(VkCommandBuffer commandBuffer);

    private:
        void CreateDescriptorPool();

    private:
        Engine::VulkanContext* m_context = nullptr;
        Engine::VulkanRenderer* m_renderer = nullptr;
        VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    };
}