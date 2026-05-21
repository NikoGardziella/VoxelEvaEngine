#pragma once

#include "Engine/Core/Layer.h"
#include <Editor/EditorImGuiLayer.h>

namespace Editor {


    class EditorLayer : public Engine::Layer
    {
    public:
        EditorLayer();
        ~EditorLayer() override;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(Engine::Timestep ts) override;
        void OnEvent(Engine::Event& event) override;



    private: 

        EditorImGuiLayer m_imguiLayer;
    };


}
