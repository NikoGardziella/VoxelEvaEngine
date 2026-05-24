#include "VoxelMesher.h"

namespace Engine
{
    namespace
    {
        constexpr glm::ivec3 FaceDirections[6] =
        {
            {  1,  0,  0 },
            { -1,  0,  0 },
            {  0,  1,  0 },
            {  0, -1,  0 },
            {  0,  0,  1 },
            {  0,  0, -1 }
        };

        constexpr glm::vec3 FaceVertices[6][4] =
        {
            {
                { 1.0f, 0.0f, 0.0f },
                { 1.0f, 1.0f, 0.0f },
                { 1.0f, 1.0f, 1.0f },
                { 1.0f, 0.0f, 1.0f }
            },
            {
                { 0.0f, 1.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f },
                { 0.0f, 1.0f, 1.0f }
            },
            {
                { 1.0f, 1.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f },
                { 0.0f, 1.0f, 1.0f },
                { 1.0f, 1.0f, 1.0f }
            },
            {
                { 0.0f, 0.0f, 0.0f },
                { 1.0f, 0.0f, 0.0f },
                { 1.0f, 0.0f, 1.0f },
                { 0.0f, 0.0f, 1.0f }
            },
            {
                { 0.0f, 0.0f, 1.0f },
                { 1.0f, 0.0f, 1.0f },
                { 1.0f, 1.0f, 1.0f },
                { 0.0f, 1.0f, 1.0f }
            },
            {
                { 0.0f, 1.0f, 0.0f },
                { 1.0f, 1.0f, 0.0f },
                { 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f }
            }
        };
    }

    Mesh VoxelMesher::BuildMesh(const VoxelChunk& chunk)
    {
        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> indices;

        for (int z = 0; z < VoxelChunk::SizeZ; z++)
        {
            for (int y = 0; y < VoxelChunk::SizeY; y++)
            {
                for (int x = 0; x < VoxelChunk::SizeX; x++)
                {
                    if (!chunk.IsSolid(x, y, z))
                        continue;

                    glm::vec3 voxelPosition = glm::vec3(x, y, z);

                    for (int face = 0; face < 6; face++)
                    {
                        glm::ivec3 direction = FaceDirections[face];

                        if (chunk.IsSolid(x + direction.x, y + direction.y, z + direction.z))
                            continue;

                        const Voxel& voxel = chunk.GetVoxel(x, y, z);

                        glm::vec3 color = {
                            voxel.Color.r / 255.0f,
                            voxel.Color.g / 255.0f,
                            voxel.Color.b / 255.0f
                        };

                        if (face == 4)
                            color *= 1.15f;

                        if (face == 5)
                            color *= 0.65f;

                        color = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));

                        AddFace(vertices, indices, voxelPosition, face, color);
                    }
                }
            }
        }

        return Mesh(vertices, indices);
    }

    void VoxelMesher::AddFace(std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices, const glm::vec3& voxelPosition, int faceIndex, const glm::vec3& color)
    {
        uint32_t startIndex = static_cast<uint32_t>(vertices.size());

        vertices.push_back({ voxelPosition + FaceVertices[faceIndex][0], color });
        vertices.push_back({ voxelPosition + FaceVertices[faceIndex][1], color });
        vertices.push_back({ voxelPosition + FaceVertices[faceIndex][2], color });
        vertices.push_back({ voxelPosition + FaceVertices[faceIndex][3], color });

        indices.push_back(startIndex + 0);
        indices.push_back(startIndex + 1);
        indices.push_back(startIndex + 2);

        indices.push_back(startIndex + 2);
        indices.push_back(startIndex + 3);
        indices.push_back(startIndex + 0);
    }
}