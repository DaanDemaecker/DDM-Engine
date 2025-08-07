#include "CameraComponent.h"
#include "../Includes/GLMIncludes.h"
#include "TransformComponent.h"
#include "Engine/Window.h"

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
	UpdateViewMatrix();
}

void DDM3::CameraComponent::UpdateUniformBuffer(UniformBufferObject& buffer, VkExtent2D extent)
{
	// Set buffer view matrix
	buffer.view = m_ViewMatrix;

	if (m_ShouldUpdateProjection)
	{
		UpdateProjectionMatrix();
	}

	buffer.proj = m_ProjectionMatrix;
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

glm::mat4& DDM3::CameraComponent::GetProjectionMatrix()
{
	return m_ProjectionMatrix;
}

glm::mat4* DDM3::CameraComponent::GetProjectionMatrixPointer()
{
	return &m_ProjectionMatrix;
}

glm::mat4& DDM3::CameraComponent::GetViewMatrix()
{
	return m_ViewMatrix;
}

glm::mat4* DDM3::CameraComponent::GetViewMatrixPointer()
{
	return &m_ViewMatrix;
}

void DDM3::CameraComponent::UpdateViewMatrix()
{
	auto transform = GetTransform();

	// Create rotation matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(glm::conjugate(transform->GetWorldRotation()));

	// Create translation matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -transform->GetWorldPosition());

	// Multiply matrices (apply rotation first, then translation)
	m_ViewMatrix = rotationMatrix * translationMatrix;
}

void DDM3::CameraComponent::UpdateProjectionMatrix()
{
	auto& windowStruct = Window::GetInstance().GetWindowStruct();

	// Set the projection matrix
	m_ProjectionMatrix = glm::perspective(m_FovAngle, windowStruct.Width / static_cast<float>(windowStruct.Height), 0.001f, 100.0f);

	m_ProjectionMatrix[1][1] *= -1;
	m_ProjectionMatrix[2][2] *= -1;
	m_ProjectionMatrix[2][3] *= -1;
}
