#pragma once

#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"
#include "Instrumentor.h"

extern Engine::Application* CreateApplication();

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    Engine::Log::Init();

    ENGINE_INFO("VoxelEvaEngine starting");

    Engine::Application* app = CreateApplication();
    EE_PROFILE_BEGIN_SESSION("Runtimep", "EvaEngineProfile-runtime.json");
    app->Run();
	EE_PROFILE_END_SESSION();
    delete app;

    ENGINE_INFO("VoxelEvaEngine shutting down");

    return 0;
}