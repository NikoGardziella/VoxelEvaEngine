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
#include "Engine/Core/EntryPoint.h"
#include <Engine/Renderer/GraphicsContext.h>
#include <Engine/Platform/Vulkan/VulkanContext.h>
#include "Engine/Core/Window.h"

namespace Editor {



    EditorLayer::EditorLayer()
        : Engine::Layer("EditorLayer")
    {
    }

    EditorLayer::~EditorLayer()
    {
    }

    void EditorLayer::OnAttach()
    {

        Engine::Window& window = Engine::Application::Get().GetWindow();
        Engine::GraphicsContext* context = window.GetGraphicsContext();

        Engine::VulkanContext* vulkanContext = static_cast<Engine::VulkanContext*>(context);

        m_imguiLayer.OnAttach(vulkanContext);
        APP_INFO("EditorLayer attached");
    }

    void EditorLayer::OnDetach()
    {
        APP_INFO("EditorLayer detached");
    }

    void EditorLayer::OnUpdate(Engine::Timestep ts)
    {
        (void)ts;

        if (Engine::Input::IsKeyDown(Engine::Key::Space))
        {
            APP_INFO("Space pressed this frame");
        }

        if (Engine::Input::IsMouseButtonDown(Engine::Mouse::ButtonLeft))
        {
            auto [x, y] = Engine::Input::GetMousePosition();
            APP_INFO("Left mouse clicked at {}, {}", x, y);
        }

        if (Engine::Input::IsMouseButtonReleased(Engine::Mouse::ButtonLeft))
        {
            APP_INFO("Left mouse released");
        }
    }

    void EditorLayer::OnEvent(Engine::Event& event)
    {
        //APP_TRACE("EditorLayer event: {}", event.ToString());
    }

}