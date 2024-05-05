#include "CameraComponent.h"
#include "../Includes/GLMIncludes.h"
#include "TransformComponent.h"

D3D::CameraComponent::CameraComponent()
{
	m_FovAngle = glm::radians(m_DefaultAngleDegrees);
}

void D3D::CameraComponent::SetFovAngleDegrees(float angle)
{
	m_FovAngle = glm::radians(angle);
}

void D3D::CameraComponent::LateUpdate()
{
	UpdateMatrix();
}

void D3D::CameraComponent::UpdateUniformBuffer(UniformBufferObject& buffer, VkExtent2D extent)
{
	// Set buffer view matrix
	buffer.view = m_Matrix;

	// Set the projection matrix
	buffer.proj = glm::perspective(m_FovAngle, extent.width / static_cast<float>(extent.height), 0.001f, 100.0f);

	buffer.proj[1][1] *= -1;

	buffer.proj[2][2] *= -1;
	buffer.proj[2][3] *= -1;
}

void D3D::CameraComponent::RenderSkybox()
{
	if (m_pSkyBox == nullptr)
	{
		m_pSkyBox = GetComponent<D3D::SkyBoxComponent>();
	}

	if (m_pSkyBox != nullptr)
	{
		m_pSkyBox->RenderSkyBox();
	}
}

void D3D::CameraComponent::UpdateMatrix()
{
	auto transform = GetTransform();

	// Create rotation matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(glm::conjugate(transform->GetWorldRotation()));

	// Create translation matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -transform->GetWorldPosition());

	// Multiply matrices (apply rotation first, then translation)
	m_Matrix = rotationMatrix * translationMatrix;
}
