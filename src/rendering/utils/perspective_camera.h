#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glfw/glfw3.h>

#include "core/input.h"

class PerspectiveCamera {
public:
    PerspectiveCamera(float fov, float aspectRatio, float zNear, float zFar);

    const glm::mat4& getModelMatrix() const { return m_ModelMatrix; }
    const glm::mat4& getViewMatrix() const { return m_ViewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
    const glm::mat4& getViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

    const glm::vec3& getPosition() const { return m_Position; }
    const glm::vec3& getFrontVector() const { return m_Front; }
    const glm::vec3& getRightVector() const { return m_Right; }

    void setProjection(float fov, float aspectRatio, float zNear, float zFar);
    void setPosition(glm::vec3 pos);
    void setRotation(float pitch, float yaw) { m_Pitch = pitch; m_Yaw = yaw; updateCameraVectors(); }

private:
    void recalculateProjection();
    void recalculateView();
    void updateCameraVectors();

private:
    glm::mat4 m_ModelMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewProjectionMatrix;

    // Euler angles
    float m_Yaw = -90.0f;
    float m_Pitch = 0.0f;

    // Camera attributes
    glm::vec3 m_Front = { 0.0f, 0.0f, -1.0f };
    glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };
    glm::vec3 m_Right = { 1.0f, 0.0f, 0.0f };
    //glm::vec3 m_WorldUp;

    float m_Zoom = 45.0f;
    float m_AspectRatio = 1.778f;
    float m_Near = 0.1f, m_Far = 100.0f;

    glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
};
