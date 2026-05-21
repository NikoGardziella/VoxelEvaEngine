#include "Engine/Platform/Vulkan/VulkanContext.h"


namespace Editor
{
    class EditorImGuiLayer
    {
    public:
        void OnAttach(Engine::VulkanContext* context);
        void OnDetach();

        void Begin();
        void End();

    private:
        void CreateDescriptorPool();

    private:
        Engine::VulkanContext* m_context = nullptr;
        VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    };
}