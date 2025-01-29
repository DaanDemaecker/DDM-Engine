#include "CameraComponent.h"
#include "../Includes/GLMIncludes.h"
#include "TransformComponent.h"

DDM3::CameraComponent::CameraComponent()
{
	m_FovAngle = glm::radians(m_DefaultAngleDegrees);
}

void DDM3::CameraComponent::SetFovAngleDegrees(float angle)
{
	m_FovAngle = glm::radians(angle);
}

void DDM3::CameraComponent::LateUpdate()
{
	UpdateMatrix();
}

void DDM3::CameraComponent::UpdateUniformBuffer(UniformBufferObject& buffer, VkExtent2D extent)
{
	// Set buffer view matrix
	buffer.view = m_Matrix;

	// Set the projection matrix
	buffer.proj = glm::perspective(m_FovAngle, extent.width / static_cast<float>(extent.height), 0.001f, 100.0f);

	buffer.proj[1][1] *= -1;

	buffer.proj[2][2] *= -1;
	buffer.proj[2][3] *= -1;
}

void DDM3::CameraComponent::RenderSkybox()
{
	if (m_pSkyBox == nullptr)
	{
		m_pSkyBox = GetComponent<DDM3::SkyBoxComponent>();
	}

	if (m_pSkyBox != nullptr)
	{
		m_pSkyBox->RenderSkyBox();
	}
}

void DDM3::CameraComponent::UpdateMatrix()
{
	auto transform = GetTransform();

	// Create rotation matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(glm::conjugate(transform->GetWorldRotation()));

	// Create translation matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -transform->GetWorldPosition());

	// Multiply matrices (apply rotation first, then translation)
	m_Matrix = rotationMatrix * translationMatrix;
}
