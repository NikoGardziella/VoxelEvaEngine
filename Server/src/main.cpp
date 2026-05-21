#include "Engine/Core/Log.h"

int main()
{
    Engine::Log::Init();

    APP_INFO("VoxelEvaEngine server running");

    return 0;
}