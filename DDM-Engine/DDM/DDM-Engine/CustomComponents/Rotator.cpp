// Rotator.cpp

// Header include
#include "Rotator.h"

// File includes
#include "DDM-Engine/Includes/GLMIncludes.h"

#include "DDM-Engine/Managers/TimeManager.h"

#include "DDM-Engine/EngineComponents/Transform.h"

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
