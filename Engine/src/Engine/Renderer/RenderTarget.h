#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Engine
{
    class RenderTarget
    {
    public:
        virtual ~RenderTarget() = default;

        virtual void Begin(VkCommandBuffer commandBuffer) = 0;
        virtual void End(VkCommandBuffer commandBuffer) = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual bool IsValid() const = 0;

        virtual VkExtent2D GetExtent() const = 0;
        virtual VkFormat GetColorFormat() const = 0;
        virtual VkRenderPass GetRenderPass() const = 0;
        virtual VkFramebuffer GetFramebuffer() const = 0;
    };
}