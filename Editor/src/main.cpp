#include "EditorLayer.h"

#include "Engine/Core/Application.h"

namespace Editor {


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

}
    Engine::Application* CreateApplication()
    {
        return new Editor::EditorApplication();
    }