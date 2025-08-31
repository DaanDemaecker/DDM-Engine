// Rotator.cpp

// Header include
#include "Rotator.h"

// File includes
#include "Includes/GLMIncludes.h"

#include "Managers/TimeManager.h"

#include "Components/Transform.h"

void DDM::RotatorComponent::Update()
{
	// Convert rotation speed to radians
	float rotSpeed{ glm::radians(m_RotationSpeed) };

	// Calculate rotation amount based on delta time
	float rotAmount{ rotSpeed * TimeManager::GetInstance().GetDeltaTime() };

	GetTransform()->Rotate(m_RotationAxis, rotAmount);
}

void DDM::RotatorComponent::SetRotAxis(glm::vec3& axis)
{
	m_RotationAxis = glm::normalize(axis);
}

void DDM::RotatorComponent::SetRotAxis(glm::vec3&& axis)
{
	// Propagate to main lvalue function
	SetRotAxis(axis);
}
