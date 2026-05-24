#pragma once

#include "Mesh.h"

namespace Engine
{
    class MeshFactory
    {
    public:
        static Mesh CreateCube(float size = 1.0f);
    };
}