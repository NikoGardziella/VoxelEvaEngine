#pragma once

#include <cstdint>

namespace Engine
{
    using EntityID = uint64_t;
    using SimulationTick = uint64_t;

    constexpr EntityID InvalidEntityID = 0;

    struct SceneTickContext
    {
        SimulationTick Tick = 0;
        float FixedDeltaTime = 1.0f / 60.0f;
    };
}