#include "../Includes/GLMIncludes.h"
#include "RotatorComponent.h"
#include "../Managers/TimeManager.h"

#include "TransformComponent.h"

void DDM3::RotatorComponent::Update()
{
	float rotSpeed{ glm::radians(m_RotationSpeed) };

	float rotAmount{ rotSpeed * TimeManager::GetInstance().GetDeltaTime() };

	glm::vec3 up{ glm::vec3( 0, 1, 0 ) };

	GetTransform()->Rotate(up, rotAmount);
}
