#include "stdafx.h"

#include "RotatorComponent.h"
#include "TimeManager.h"

#include "TransformComponent.h"

void D3D::RotatorComponent::Update()
{
	constexpr float rotSpeed{ glm::radians(90.f) };

	float rotAmount{ rotSpeed * TimeManager::GetInstance().GetDeltaTime() };

	GetTransform()->Rotate(0, rotAmount, 0);
}
