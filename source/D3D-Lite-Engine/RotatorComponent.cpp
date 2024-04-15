#include "GLMIncludes.h"
#include "RotatorComponent.h"
#include "TimeManager.h"

#include "TransformComponent.h"

void D3D::RotatorComponent::Update()
{
	constexpr float rotSpeed{ glm::radians(50.f) };

	float rotAmount{ rotSpeed * TimeManager::GetInstance().GetDeltaTime() };

	glm::vec3 up{ glm::vec3( 0, 1, 0 ) };

	GetTransform()->Rotate(up, rotAmount);
}
