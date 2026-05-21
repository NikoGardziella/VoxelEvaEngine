#include "EditorLayer.h"

#include "Engine/Core/Application.h"
#include "Engine/Core/EntryPoint.h"

class EditorApplication : public Engine::Application
{
public:
    EditorApplication()
    {
        PushLayer(new EditorLayer());
    }

    ~EditorApplication()
    {
    }
};

Engine::Application* CreateApplication()
{
    return new EditorApplication();
}