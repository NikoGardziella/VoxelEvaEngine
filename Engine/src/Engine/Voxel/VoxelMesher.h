#pragma once

#include "VoxelChunk.h"
#include "Engine/Renderer/Mesh/Mesh.h"

namespace Engine
{
    class VoxelMesher
    {
    public:
        static Mesh BuildMesh(const VoxelChunk& chunk);

    private:
        static void AddFace(std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices, const glm::vec3& voxelPosition, int faceIndex, const glm::vec3& color, const uint32_t pickID);
    };
}