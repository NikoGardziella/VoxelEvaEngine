#pragma once

#include "Engine/Core/Layer.h"

class EditorLayer : public Engine::Layer
{
public:
    EditorLayer();
    ~EditorLayer() override;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(Engine::Timestep ts) override;
    void OnEvent(Engine::Event& event) override;
};