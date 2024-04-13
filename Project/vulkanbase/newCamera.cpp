#include "newCamera.h"
#include <iostream>
#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>
#include <glm-1.0.0/glm/gtc/matrix_transform.hpp>
#define GLM_FORCE_RADIANS

void Camera::keyEvent(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		//m_CameraPos.x = std::max(3.0f, m_CameraPos.x - 0.2f);
		m_Position.x -= 0.2f;
	}
	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		m_Position.x = std::min(30.0f, m_Position.x + 0.2f);
	}
	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		m_Position.y = std::max(3.0f, m_Position.y - 0.2f);
	}
	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		m_Position.y = std::min(30.0f, m_Position.y + 0.2f);
	}

}

void Camera::mouseMove(GLFWwindow* window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (state == GLFW_PRESS)
	{
		float dx = static_cast<float>(xpos) - m_DragStart.x;
		if (dx > 0) {
			m_Yaw += 0.01f;
		}
		else {
			m_Yaw -= 0.01f;
		}
	}
}

void Camera::mouseEvent(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		std::cout << "right mouse button pressed\n";
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		m_DragStart.x = static_cast<float>(xpos);
		m_DragStart.y = static_cast<float>(ypos);
	}
}

void Camera::CalculateViewMat()
{
	glm::mat4 rotation = glm::rotate(glm::mat4(1.f), m_Yaw, glm::vec3{ 0.f, 0.f, 1.f });
	rotation = glm::rotate(rotation, m_Pitch, glm::vec3{ 1.f,0.f,0.f });

	m_Forward = glm::normalize(rotation[2]);
	m_Right = glm::normalize(glm::cross(glm::vec3{ 0.f, 0.f, 1.f }, m_Forward));

	m_ViewMat =  static_cast<glm::mat4>(glm::lookAt(m_Position, m_Position + m_Forward, glm::vec3{ 0.f, 0.f, 1.f }));
}

void Camera::CalculateProjMat()
{
	m_ProjMat = glm::perspective(m_FieldOfView, m_AspectRatio, m_NearPlane, m_FarPlane);

}
