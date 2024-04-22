#include "newCamera.h"
#include <iostream>
#include "Time.h"
#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>
#include <glm-1.0.0/glm/gtc/matrix_transform.hpp>
#define GLM_FORCE_RADIANS
//void Camera::keyEvent(int key, int scancode, int action, int mods)
//{
//    const float cameraspeed{ 0.2f };
//    if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
//    {
//        m_Position += m_Forward * cameraspeed;
//    }
//    if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
//    {
//        m_Position -= m_Forward * cameraspeed;
//    }
//    if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
//    {
//        m_Position += m_Right * cameraspeed;
//    }
//    if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
//    {
//        m_Position -= m_Right * cameraspeed;
//    }
//
//}
//
//void Camera::mouseMove(GLFWwindow* window, double xpos, double ypos)
//{
//    const float RotateSpeed{ 0.0005f };
//
//    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
//    if (state == GLFW_PRESS)
//    {
//        float dx = static_cast<float>(xpos) - m_DragStart.x;
//        m_Yaw += dx * RotateSpeed * Time::GetElapesedSec();
//
//        float dy = static_cast<float>(ypos) - m_DragStart.y;
//        m_Pitch += dy * RotateSpeed * Time::GetElapesedSec();
//
//        m_DragStart = glm::vec2{ xpos, ypos };
//    }
//}
//
//void Camera::mouseEvent(GLFWwindow* window, int button, int action, int mods)
//{
//    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
//    {
//        double xpos, ypos;
//        glfwGetCursorPos(window, &xpos, &ypos);
//        m_DragStart.x = static_cast<float>(xpos);
//        m_DragStart.y = static_cast<float>(ypos);
//    }
//}
//
//void Camera::CalculateViewMat()
//{
//    glm::mat4 rotation = glm::rotate(glm::mat4(1.f), m_Yaw, m_Up);
//    rotation = glm::rotate(rotation, m_Pitch, glm::vec3{ 1.f,0.f,0.f });
//
//    m_Forward = glm::normalize(rotation[2]);
//    m_Right = glm::normalize(glm::cross(m_Up, m_Forward));
//
//    //return  glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
//    m_ViewMat =  glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
//}
//
//void Camera::CalculateProjMat()
//{
//    //return glm::perspective(m_FieldOfView, m_AspectRatio, m_NearPlane, m_FarPlane);
//    m_ProjMat = glm::perspective(m_FieldOfView, m_AspectRatio, m_NearPlane, m_FarPlane);
//
//}