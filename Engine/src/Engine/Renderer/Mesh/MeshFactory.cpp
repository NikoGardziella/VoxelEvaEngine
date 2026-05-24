#include "MeshFactory.h"

namespace Engine
{
    Mesh MeshFactory::CreateCube(float size)
    {
        float h = size * 0.5f;

        std::vector<MeshVertex> vertices =
        {
            { { -h, -h, -h }, { 1.0f, 0.0f, 0.0f } },
            { {  h, -h, -h }, { 0.0f, 1.0f, 0.0f } },
            { {  h,  h, -h }, { 0.0f, 0.0f, 1.0f } },
            { { -h,  h, -h }, { 1.0f, 1.0f, 0.0f } },

            { { -h, -h,  h }, { 1.0f, 0.0f, 1.0f } },
            { {  h, -h,  h }, { 0.0f, 1.0f, 1.0f } },
            { {  h,  h,  h }, { 1.0f, 1.0f, 1.0f } },
            { { -h,  h,  h }, { 0.3f, 0.3f, 0.3f } }
        };

        std::vector<uint32_t> indices =
        {
            0, 1, 2, 2, 3, 0,
            4, 6, 5, 6, 4, 7,
            0, 4, 5, 5, 1, 0,
            3, 2, 6, 6, 7, 3,
            1, 5, 6, 6, 2, 1,
            0, 3, 7, 7, 4, 0
        };

        return Mesh(vertices, indices);
    }
}