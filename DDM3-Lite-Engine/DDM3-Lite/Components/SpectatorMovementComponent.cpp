#include "SpectatorMovementComponent.h"
#include "TransformComponent.h"
#include "../Includes/GLMIncludes.h"
#include "../Managers/TimeManager.h"

#include <iostream>
#include "../Includes/GLFWIncludes.h"
#include "../Includes/ImGuiIncludes.h"
#include "../Engine/Window.h"
#include "Managers/InputManager.h"

DDM3::SpectatorMovementComponent::SpectatorMovementComponent()
{

}

void DDM3::SpectatorMovementComponent::Update()
{
	if (m_pTransform == nullptr)
	{
		m_pTransform = GetComponent<TransformComponent>().get();
		
		auto rotation = m_pTransform->GetWorldRotation();

		glm::vec3 eulerAngles = glm::eulerAngles( rotation );
		
		m_TotalPitch = eulerAngles.x;

		m_TotalYaw = eulerAngles.y;
	}


	auto& input{ InputManager::GetInstance() };

	glm::vec3 direction{};

	auto window = Window::GetInstance().GetWindowStruct().pWindow;

	auto deltaTime = TimeManager::GetInstance().GetDeltaTime();

	// Determine movement direction based on key presses
	if (input.GetKeyPressed(GLFW_KEY_W))
	{
		//direction += transform->GetForward() * m_Speed * deltaTime;
		direction += m_pTransform->GetForward();
	}
	if (input.GetKeyPressed(GLFW_KEY_S))
	{
		//direction -= transform->GetForward() * m_Speed * deltaTime;
		direction -= m_pTransform->GetForward();
	}
	if (input.GetKeyPressed(GLFW_KEY_A))
	{
		//direction -= transform->GetRight() * m_Speed * deltaTime;
		direction -= m_pTransform->GetRight();
	}
	if (input.GetKeyPressed(GLFW_KEY_D))
	{
		//direction += transform->GetRight() * m_Speed * deltaTime;
		direction += m_pTransform->GetRight();
	}

	glm::normalize(direction);

	direction *= m_Speed * deltaTime;


	// Translate the object based on the rotated movement direction
	m_pTransform->Translate(direction);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		auto& mouseDelta = input.GetMouseDelta();


		m_TotalPitch += static_cast<float>(mouseDelta.y * m_AngularSpeed);
		m_TotalYaw += static_cast<float>(mouseDelta.x * m_AngularSpeed);

		// Rotate the camera based on mouse movement
		m_pTransform->SetWorldRotation(m_TotalPitch, m_TotalYaw, 0);
	}
}