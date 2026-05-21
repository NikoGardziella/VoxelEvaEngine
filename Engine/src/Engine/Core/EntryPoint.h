#pragma once

#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"

extern Engine::Application* CreateApplication();

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    Engine::Log::Init();

    ENGINE_INFO("VoxelEvaEngine starting");

    Engine::Application* app = CreateApplication();
    app->Run();
    delete app;

    ENGINE_INFO("VoxelEvaEngine shutting down");

    return 0;
}