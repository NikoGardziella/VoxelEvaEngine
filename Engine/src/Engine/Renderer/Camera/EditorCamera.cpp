#include "EditorCamera.h"

#include "Engine/Core/Input.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Engine/Core/KeyCodes.h"
#include <Engine/Core/Log.h>

namespace Engine
{
    EditorCamera::EditorCamera()
    {
        RecalculateViewProjection();
    }

    void EditorCamera::OnUpdate(Engine::Timestep timestep, bool viewportHovered)
    {
        if (!viewportHovered)
            return;

        glm::vec2 mouse = Input::GetMouseGlmPosition();
        glm::vec2 delta = mouse - m_lastMousePosition;
        m_lastMousePosition = mouse;

        delta *= 0.003f;

        if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
        {
            MousePan(delta);
        }

        if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
        {
            MouseRotate(delta);
        }

        if (Input::IsKeyPressed(Key::W))
            m_focalPoint += GetForwardDirection() * static_cast<float>(timestep) * 5.0f;

        if (Input::IsKeyPressed(Key::S))
            m_focalPoint -= GetForwardDirection() * static_cast<float>(timestep) * 5.0f;

        if (Input::IsKeyPressed(Key::A))
            m_focalPoint -= GetRightDirection() * static_cast<float>(timestep) * 5.0f;

        if (Input::IsKeyPressed(Key::D))
            m_focalPoint += GetRightDirection() * static_cast<float>(timestep) * 5.0f;

        RecalculateViewProjection();
    }

    void EditorCamera::MousePan(const glm::vec2& delta)
    {
        float speed = m_distance * 0.8f;

        m_focalPoint -= GetRightDirection() * delta.x * speed;
        m_focalPoint += GetUpDirection() * delta.y * speed;
    }

    void EditorCamera::MouseRotate(const glm::vec2& delta)
    {
        m_yaw += delta.x * m_rotationSpeed * 100.0f;
        m_pitch += delta.y * m_rotationSpeed * 100.0f;

        if (m_pitch > 89.0f)
            m_pitch = 89.0f;

        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

    void EditorCamera::MouseZoom(float delta)
    {
        m_distance -= delta * m_zoomSpeed;
        m_distance = glm::max(m_distance, 0.5f);
    }

    void EditorCamera::OnResize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;

        m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        RecalculateViewProjection();
    }

    void EditorCamera::SetPosition(const glm::vec3& position)
    {
        m_position = position;
        RecalculateViewProjection();
    }

    void EditorCamera::SetZoom(float zoom)
    {
        m_zoom = zoom;
        ClampZoom();
        RecalculateViewProjection();
    }

    void EditorCamera::RecalculateViewProjection()
    {
        m_position = CalculatePosition();

        glm::vec3 forward = GetForwardDirection();
        glm::vec3 up = GetUpDirection();

        m_view = glm::lookAt(m_position, m_focalPoint, up);

        m_projection = glm::perspective(glm::radians(45.0f), m_aspectRatio, 0.1f, 1000.0f);
        m_projection[1][1] *= -1.0f;

        m_viewProjection = m_projection * m_view;
    }

    glm::vec3 EditorCamera::GetForwardDirection() const
    {
        float pitchRad = glm::radians(m_pitch);
        float yawRad = glm::radians(m_yaw);

        glm::vec3 direction{};
        direction.x = cos(pitchRad) * cos(yawRad);
        direction.y = cos(pitchRad) * sin(yawRad);
        direction.z = sin(pitchRad);

        return glm::normalize(direction);
    }

    glm::vec3 EditorCamera::GetRightDirection() const
    {
        return glm::normalize(glm::cross(GetForwardDirection(), glm::vec3(0.0f, 0.0f, 1.0f)));
    }

    glm::vec3 EditorCamera::GetUpDirection() const
    {
        return glm::normalize(glm::cross(GetRightDirection(), GetForwardDirection()));
    }

    glm::vec3 EditorCamera::CalculatePosition() const
    {
        return m_focalPoint - GetForwardDirection() * m_distance;
    }

    void EditorCamera::ClampZoom()
    {
        if (m_zoom < m_minZoom)
            m_zoom = m_minZoom;

        if (m_zoom > m_maxZoom)
            m_zoom = m_maxZoom;
    }
}