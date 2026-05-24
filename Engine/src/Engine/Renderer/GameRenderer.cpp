#include "GameRenderer.h"
#include "Engine/Platform/Vulkan/VulkanPipeline.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Camera/CameraPushConstants.h"

namespace Engine
{
    void GameRenderer::Init(VkDevice device)
    {
        m_device = device;
    }

    void GameRenderer::Shutdown()
    {
        m_pipeline = nullptr;
        m_device = VK_NULL_HANDLE;
    }

    void GameRenderer::SetMeshBuffer(VulkanMeshBuffer* meshBuffer)
    {
        m_meshBuffer = meshBuffer;
    }
    void GameRenderer::SetViewProjection(const glm::mat4& viewProjection)
    {
        m_viewProjection = viewProjection;
    }

    void GameRenderer::SetPipeline(VulkanPipeline* pipeline)
    {
        m_pipeline = pipeline;
    }

    void GameRenderer::Render(VkCommandBuffer commandBuffer, RenderTarget& target)
    {
        if (commandBuffer == VK_NULL_HANDLE)
            return;

        if (!target.IsValid())
            return;

        if (!m_pipeline)
            return;

        target.Begin(commandBuffer);

        DrawMesh(commandBuffer);

        target.End(commandBuffer);
    }

    void GameRenderer::DrawMesh(VkCommandBuffer commandBuffer)
    {
        if (!m_pipeline)
            return;

        if (!m_meshBuffer || !m_meshBuffer->IsValid())
            return;

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipeline());

        glm::mat4 model = glm::mat4(1.0f);

        Engine::CameraPushConstants pushConstants{};
        pushConstants.MVP = m_viewProjection * model;

        vkCmdPushConstants(
            commandBuffer,
            m_pipeline->GetPipelineLayout(),
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(Engine::CameraPushConstants),
            &pushConstants
        );

        m_meshBuffer->Draw(commandBuffer);
    }
}