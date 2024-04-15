#include "SpectatorMovementComponent.h"
#include "TransformComponent.h"
#include "GLMIncludes.h"
#include "TimeManager.h"

#include <iostream>
#include "GLFWIncludes.h"
#include "Window.h"

D3D::SpectatorMovementComponent::SpectatorMovementComponent()
{
	glfwGetCursorPos(Window::GetInstance().GetWindowStruct().pWindow, &m_PrevXPos, &m_PrevYPos);
}

void D3D::SpectatorMovementComponent::Update()
{
	glm::vec3 movement{};

	auto window = Window::GetInstance().GetWindowStruct().pWindow;

	
	movement.z -= (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
	movement.z += (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
	movement.x -= (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
	movement.x += (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	double deltaX{ xpos - m_PrevXPos };
	double deltaY{ ypos - m_PrevYPos };

	m_PrevXPos = xpos;
	m_PrevYPos = ypos;

	auto transform{ GetTransform() };
	auto deltaTime{ TimeManager::GetInstance().GetDeltaTime() };

	transform->Rotate(glm::vec3{0, 1, 0}, static_cast<float>(deltaX * m_AngularSpeed * deltaTime));

	transform->Rotate(transform->GetRight(), static_cast<float>(-deltaY * m_AngularSpeed * deltaTime));

	transform->Translate((transform->GetWorldRotation() * movement) * m_Speed * TimeManager::GetInstance().GetDeltaTime());
}
