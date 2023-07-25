#include "stdafx.h"
#include "CameraComponent.h"

D3D::CameraComponent::CameraComponent()
{
	m_FovAngle = glm::radians(m_DefaultAngleDegrees);
}

void D3D::CameraComponent::SetFovAngleDegrees(float angle)
{
	m_FovAngle = glm::radians(angle);
}
