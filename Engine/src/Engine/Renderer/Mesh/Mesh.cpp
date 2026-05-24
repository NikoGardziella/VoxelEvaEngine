#include "Mesh.h"

namespace Engine
{
    Mesh::Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices) : m_vertices(vertices), m_indices(indices)
    {
    }
}