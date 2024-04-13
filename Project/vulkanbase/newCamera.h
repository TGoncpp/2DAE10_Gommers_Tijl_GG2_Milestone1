#pragma once
#include <glm-1.0.0/glm/glm.hpp>
#include <GLFW/glfw3.h>

//struct GFLWwindow;

class Camera
{
public:
	Camera(const glm::vec3& position, float fov, float ar)
		:m_Position{position}, m_AspectRatio{ar}, m_FieldOfView{fov}{}
	~Camera() = default;

	void keyEvent(int key, int scancode, int action, int mods);
	void mouseMove(GLFWwindow* window, double xpos, double ypos);
	void mouseEvent(GLFWwindow* window, int button, int action, int mods);

	glm::vec3 GetPosition()const { return m_Position; };
	float GetfieldOfView()const { return m_FieldOfView; };
	float GetAspectRatio()const { return m_AspectRatio; };
	float GetNearPlane()const { return m_NearPlane; };
	float GetFarPlane()const { return m_FarPlane; };
	glm::mat4 GetProjMat()const { return m_ProjMat; };

	void CalculateViewMat();
	void CalculateProjMat();

private:
	glm::vec3 m_Position{};
	glm::vec3 m_WorldPosition{};
	float m_FieldOfView{ 1.f };
	float m_AspectRatio{ 1.f };
	const float m_NearPlane{ 0.1f };
	const float m_FarPlane{ 10.f };

	glm::mat4 m_ViewMat{};
	glm::mat4 m_ProjMat{};
	glm::vec3 m_Forward{};
	glm::vec3 m_Right{};
	glm::vec3 m_Up{};
	glm::vec2 m_DragStart{};
	float m_Yaw{};
	float m_Pitch{};

};