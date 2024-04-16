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
	glm::vec3 direction{};

	auto window = Window::GetInstance().GetWindowStruct().pWindow;

	auto transform = GetTransform();
	auto deltaTime = TimeManager::GetInstance().GetDeltaTime();

	// Determine movement direction based on key presses
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		//direction += transform->GetForward() * m_Speed * deltaTime;
		direction.z += 1;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		//direction -= transform->GetForward() * m_Speed * deltaTime;
		direction.z -= 1;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		//direction -= transform->GetRight() * m_Speed * deltaTime;
		direction.x += 1;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		//direction += transform->GetRight() * m_Speed * deltaTime;
		direction.x -= 1;
	}

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	double deltaX = xpos - m_PrevXPos;
	double deltaY = ypos - m_PrevYPos;

	m_PrevXPos = xpos;
	m_PrevYPos = ypos;


	glm::normalize(direction);

	// Create the rotation matrix from the quaternion
	glm::mat4 rotationMatrix = glm::mat4_cast(transform->GetWorldRotation());

	// Apply the rotation to the vector using the rotation matrix
	glm::vec4 rotatedVector = rotationMatrix * glm::vec4(direction, 0.0f);

	// Extract the rotated glm::vec3 from the glm::vec4
	direction = glm::vec3(rotatedVector);

	glm::normalize(direction);

	direction *= m_Speed * deltaTime;

	// Translate the object based on the rotated movement direction
	transform->Translate(direction);

	m_TotalPitch += static_cast<float>(deltaY * deltaTime * m_AngularSpeed);
	m_TotalYaw += static_cast<float>(deltaX * deltaTime * m_AngularSpeed);

	// Rotate the camera based on mouse movement
	transform->SetWorldRotation(m_TotalPitch, m_TotalYaw, 0);

	std::cout << "x: " << direction.x << "y: " << direction.y << "z: " << direction.z << std::endl;
}