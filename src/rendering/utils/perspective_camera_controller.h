#pragma once

#include "perspective_camera.h"

class PerspectiveCameraController {
public:
	PerspectiveCameraController(float aspectRatio);

	void OnUpdate(float ts);

	PerspectiveCamera& GetCamera() { return m_Camera; }
	const PerspectiveCamera& GetCamera() const { return m_Camera; }

private:
	float m_AspectRatio = 1.778f;
	float m_Zoom = 45.0f;

	glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 2.0f };
	float m_Yaw = -90.0f;
	float m_Pitch = 0.0f;

	// Camera options
	float m_MovementSpeed = 5.0f;
	float m_MouseSensitivity = 0.0f;

	glm::vec2 m_LastMousePosition = { 0.0f, 0.0f };
	bool m_FirstMouse = true;

	PerspectiveCamera m_Camera;
};
