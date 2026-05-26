#pragma once

#include "Engine/Core/Timestep.h"

#include <glm/glm.hpp>

namespace Engine
{
    class EditorCamera
    {
    public:
        EditorCamera();
        ~EditorCamera() = default;

        void OnUpdate(Engine::Timestep timestep, bool viewportHovered);
        void OnResize(uint32_t width, uint32_t height);

        void SetPosition(const glm::vec3& position);
        void SetZoom(float zoom);

        const glm::vec3& GetPosition() const { return m_position; }
        float GetZoom() const { return m_zoom; }

        const glm::mat4& GetProjection() const { return m_projection; }
        const glm::mat4& GetView() const { return m_view; }
        const glm::mat4& GetViewProjection() const { return m_viewProjection; }

        glm::vec3 GetForwardDirection() const;
        const glm::vec3& GetFocalPoint() const { return m_focalPoint; }

    private:
        void RecalculateViewProjection();
        void ClampZoom();

        glm::vec3 GetRightDirection() const;
        glm::vec3 GetUpDirection() const;
        glm::vec3 CalculatePosition() const;

        void MousePan(const glm::vec2& delta);
        void MouseRotate(const glm::vec2& delta);
        void MouseZoom(float delta);

    private:
        glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };

        float m_zoom = 1.0f;
        float m_minZoom = 0.1f;
        float m_maxZoom = 20.0f;

        float m_aspectRatio = 16.0f / 9.0f;

        glm::mat4 m_projection = glm::mat4(1.0f);
        glm::mat4 m_view = glm::mat4(1.0f);
        glm::mat4 m_viewProjection = glm::mat4(1.0f);

        bool m_isPanning = false;
        glm::vec2 m_lastMousePosition = { 0.0f, 0.0f };

        float m_panSpeed = 1.0f;
        float m_zoomSpeed = 0.15f;

        float m_pitch = -30.0f;
        float m_yaw = 45.0f;
        float m_distance = 5.0f;

        glm::vec3 m_focalPoint = glm::vec3(0.0f);
        float m_rotationSpeed = 0.3f;
    };
}