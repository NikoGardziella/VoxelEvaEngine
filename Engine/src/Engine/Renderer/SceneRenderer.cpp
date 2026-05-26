#include "SceneRenderer.h"

#include "Engine/Assets/AssetManager.h"
#include "Engine/Core/Log.h"
#include "Engine/Renderer/Camera/CameraPushConstants.h"
#include "Engine/Renderer/RenderTarget.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components/Core/TransformComponent.h"
#include "Engine/Scene/Components/Rendering/VoxelRendererComponent.h"
#include "Engine/Voxel/VoxelAsset.h"
#include "Engine/Voxel/VoxelMesher.h"
#include "Engine/Platform/Vulkan/VulkanPipeline.h"
#include "Engine/Scene/Entity.h"

#include "entt/entt.hpp"


namespace Engine
{
    SceneRenderer::~SceneRenderer()
    {
        Shutdown();
    }

    void SceneRenderer::Init(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
    {
        m_device = device;
        m_physicalDevice = physicalDevice;
        m_commandPool = commandPool;
        m_graphicsQueue = graphicsQueue;
    }

    void SceneRenderer::Shutdown()
    {
        m_voxelMeshCache.clear();

        m_pipeline = nullptr;
        m_device = VK_NULL_HANDLE;
        m_physicalDevice = VK_NULL_HANDLE;
        m_commandPool = VK_NULL_HANDLE;
        m_graphicsQueue = VK_NULL_HANDLE;
    }

    void SceneRenderer::SetPipeline(VulkanPipeline* pipeline)
    {
        m_pipeline = pipeline;
    }

    void SceneRenderer::SetPickingPipeline(VulkanPipeline* pipeline)
    {
        m_pickingPipeline = pipeline;
    }

    void SceneRenderer::SetViewProjection(const glm::mat4& viewProjection)
    {
        m_viewProjection = viewProjection;
    }

    void SceneRenderer::RenderScene(VkCommandBuffer commandBuffer, RenderTarget& target, Scene& scene, AssetManager& assetManager)
    {
        if (commandBuffer == VK_NULL_HANDLE)
            return;

        if (!target.IsValid())
            return;

        if (!m_pipeline)
            return;

        target.Begin(commandBuffer);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipeline());

        scene.ForEach<TransformComponent, VoxelRendererComponent>(
            [&](Engine::Entity entity, const TransformComponent& transform, const VoxelRendererComponent& voxelRenderer)
            {
                if (voxelRenderer.VoxelAsset == InvalidAssetHandle)
                    return;

                VulkanMeshBuffer* meshBuffer = GetOrCreateVoxelMeshBuffer(voxelRenderer.VoxelAsset, assetManager);

                if (!meshBuffer || !meshBuffer->IsValid())
                    return;

                DrawVoxelEntity(commandBuffer, *meshBuffer, transform.GetTransform());
            }
        );

        target.End(commandBuffer);
    }

    void SceneRenderer::RenderScenePicking(VkCommandBuffer commandBuffer, RenderTarget& target, Scene& scene, AssetManager& assetManager)
    {
        if (commandBuffer == VK_NULL_HANDLE)
            return;

        if (!target.IsValid())
            return;

        if (!m_pickingPipeline)
            return;

        target.Begin(commandBuffer);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pickingPipeline->GetPipeline());

        scene.ForEach<TransformComponent, VoxelRendererComponent>(
            [&](Engine::Entity entity, const TransformComponent& transform, const VoxelRendererComponent& voxelRenderer)
            {
                if (voxelRenderer.VoxelAsset == InvalidAssetHandle)
                    return;

                VulkanMeshBuffer* meshBuffer = GetOrCreateVoxelMeshBuffer(voxelRenderer.VoxelAsset, assetManager);

                if (!meshBuffer || !meshBuffer->IsValid())
                    return;

                DrawVoxelEntityPicking(commandBuffer, *meshBuffer, transform.GetTransform());
            }
        );

        target.End(commandBuffer);
    }
    void SceneRenderer::DrawVoxelEntityPicking(VkCommandBuffer commandBuffer, VulkanMeshBuffer& meshBuffer, const glm::mat4& transform)
    {
        CameraPushConstants pushConstants{};
        pushConstants.MVP = m_viewProjection * transform;

        vkCmdPushConstants(
            commandBuffer,
            m_pickingPipeline->GetPipelineLayout(),
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(CameraPushConstants),
            &pushConstants
        );

        meshBuffer.Draw(commandBuffer);
    }


    VulkanMeshBuffer* SceneRenderer::GetOrCreateVoxelMeshBuffer(AssetHandle handle, AssetManager& assetManager)
    {
        auto cachedIt = m_voxelMeshCache.find(handle);

        if (cachedIt != m_voxelMeshCache.end())
            return cachedIt->second.get();

        std::shared_ptr<Asset> asset = assetManager.GetAsset(handle);
        std::shared_ptr<VoxelAsset> voxelAsset = std::dynamic_pointer_cast<VoxelAsset>(asset);

        if (!voxelAsset)
        {
            ENGINE_ERROR("SceneRenderer failed to load voxel asset handle {}", static_cast<unsigned long long>(handle));
            return nullptr;
        }

        Mesh mesh = VoxelMesher::BuildMesh(voxelAsset->GetChunk());

        if (!mesh.IsValid())
        {
            ENGINE_ERROR("SceneRenderer voxel asset produced empty mesh {}", static_cast<unsigned long long>(handle));
            return nullptr;
        }

        std::unique_ptr<VulkanMeshBuffer> meshBuffer = std::make_unique<VulkanMeshBuffer>();
        meshBuffer->Init(m_device, m_physicalDevice, m_commandPool, m_graphicsQueue);
        meshBuffer->SetData(mesh);

        VulkanMeshBuffer* rawBuffer = meshBuffer.get();
        m_voxelMeshCache[handle] = std::move(meshBuffer);

        ENGINE_INFO("SceneRenderer cached voxel mesh {}: {} vertices, {} indices", static_cast<unsigned long long>(handle), mesh.GetVertexCount(), mesh.GetIndexCount());

        return rawBuffer;
    }

    void SceneRenderer::DrawVoxelEntity(VkCommandBuffer commandBuffer, VulkanMeshBuffer& meshBuffer, const glm::mat4& modelMatrix)
    {
        CameraPushConstants pushConstants{};
        pushConstants.MVP = m_viewProjection * modelMatrix;

        vkCmdPushConstants(commandBuffer, m_pipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(CameraPushConstants), &pushConstants);

        meshBuffer.Draw(commandBuffer);
    }
}