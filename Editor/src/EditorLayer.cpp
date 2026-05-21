#include "EditorLayer.h"

#include "Engine/Core/Events/KeyEvent.h"
#include "Engine/Core/Events/MouseEvent.h"
#include "Engine/Core/Log.h"
#include <Engine/Core/Input.h>
#include <GLFW/glfw3.h>
#include <glm/ext/vector_float3.hpp>


#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/MouseCodes.h"

EditorLayer::EditorLayer()
    : Engine::Layer("EditorLayer")
{
}

EditorLayer::~EditorLayer()
{
}

void EditorLayer::OnAttach()
{
    APP_INFO("EditorLayer attached");
}

void EditorLayer::OnDetach()
{
    APP_INFO("EditorLayer detached");
}

void EditorLayer::OnUpdate(Engine::Timestep ts)
{
    (void)ts;

    if (Engine::Input::IsKeyPressed(Engine::Key::Space))
    {
        APP_INFO("Space pressed");
    }

    if (Engine::Input::IsMouseButtonPressed(Engine::Mouse::ButtonLeft))
    {
        auto [x, y] = Engine::Input::GetMousePosition();
        APP_INFO("Left mouse at {}, {}", x, y);
    }
}

void EditorLayer::OnEvent(Engine::Event& event)
{
    APP_TRACE("EditorLayer event: {}", event.ToString());
}