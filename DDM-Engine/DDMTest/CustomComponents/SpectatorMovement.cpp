// SpectatorMovement.cpp

// Header include
#include "SpectatorMovement.h"

// File includes
#include "DDM-Engine/EngineComponents/Transform.h"

#include "DDM-Engine/Includes/GLMIncludes.h"

#include "DDM-Engine/Managers/TimeManager.h"
#include "DDM-Engine/Managers/InputManager.h"

#include "DDM-Engine/Includes/GLFWIncludes.h"
#include "DDM-Engine/Includes/ImGuiIncludes.h"

#include "DDM-Engine/Engine/Window.h"

// Standard library includes
#include <iostream>

DDM::SpectatorMovement::SpectatorMovement()
{

}

void DDM::SpectatorMovement::Update()
{
	// If transform is null, get the transform component and set up total pitch and yaw
	if (m_pTransform == nullptr)
	{
		// Get transform component
		m_pTransform = GetComponent<Transform>().get();

		// Extract rotation and convert to eulerangles
		auto rotation = m_pTransform->GetWorldRotation();
		glm::vec3 eulerAngles = glm::eulerAngles(rotation);

		// Set total pitch and yaw
		m_TotalPitch = eulerAngles.x;
		m_TotalYaw = eulerAngles.y;
	}

	// Get input manager instance
	auto& input{ InputManager::GetInstance() };

	// If right mouse button is pressed rotate camera
	if (input.GetMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
	{
		// Movement direction is initially zero
		glm::vec3 direction{};

		// Get window pointer
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

		if (direction.x != 0 || direction.y != 0 || direction.z != 0)
		{
			// Normalize direction and scale with speed and deltatime
			direction = glm::normalize(direction);
			direction *= m_Speed * deltaTime;

			// Translate the object based on the rotated movement direction
			m_pTransform->Translate(direction);
		}



		// Retrieve scroll delta
		auto& scrollDelta = input.GetScrollDelta();

		// If scrolled down, slow down camera
		if (scrollDelta.y < 0)
		{
			m_Speed /= m_IncreaseFactor;
		}
		// If scrolled up, speed up camera
		else if (scrollDelta.y > 0)
		{
			m_Speed *= m_IncreaseFactor;
		}


		// Retrieve mouse delta
		auto& mouseDelta = input.GetMouseDelta();

		// Add mouse delta to pitch and yaw, scaled with angular speed
		m_TotalPitch += static_cast<float>(mouseDelta.y * m_AngularSpeed);
		m_TotalYaw += static_cast<float>(mouseDelta.x * m_AngularSpeed);

		// Rotate the camera based on mouse movement
		m_pTransform->SetWorldRotation(m_TotalPitch, m_TotalYaw, 0);
	}
}