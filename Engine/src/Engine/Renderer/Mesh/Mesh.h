#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

namespace Engine
{
    struct MeshVertex
    {
        glm::vec3 Position;
        glm::vec3 Color;
    };

    class Mesh
    {
    public:
        Mesh() = default;
        Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices);

        const std::vector<MeshVertex>& GetVertices() const { return m_vertices; }
        const std::vector<uint32_t>& GetIndices() const { return m_indices; }

        uint32_t GetVertexCount() const { return static_cast<uint32_t>(m_vertices.size()); }
        uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_indices.size()); }

        bool IsValid() const { return !m_vertices.empty() && !m_indices.empty(); }

    private:
        std::vector<MeshVertex> m_vertices;
        std::vector<uint32_t> m_indices;
    };
}