#include "Engine/Core/Application.h"
#include "Engine/Core/EntryPoint.h"

class GameApplication : public Engine::Application
{
public:
    GameApplication()
    {
    }

    ~GameApplication()
    {
    }
};

Engine::Application* CreateApplication()
{
    return new GameApplication();
}