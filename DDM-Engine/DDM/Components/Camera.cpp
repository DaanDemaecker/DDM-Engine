// Camera.cpp

// Header include
#include "Camera.h"

// File includes
#include "Includes/GLMIncludes.h"
#include "Components/TransformComponent.h"
#include "Engine/Window.h"

DDM::Camera::Camera()
{ 
	// Set default FOV angle
	m_FovAngle = glm::radians(m_DefaultAngleDegrees);
}

void DDM::Camera::SetFovAngleDeg(float angle)
{
	// Convert degrees to radians and set FOV angle
	m_FovAngle = glm::radians(angle);
}

void DDM::Camera::LateUpdate()
{
	// Update projection and view matrix
	UpdateProjectionMatrix();

	UpdateViewMatrix();
}

void DDM::Camera::UpdateUniformBuffer(UniformBufferObject& buffer, VkExtent2D extent)
{
	// Set buffer view matrix
	buffer.view = m_ViewMatrix;

	// Set buffer projection matrix
	buffer.proj = m_ProjectionMatrix;
}

void DDM::Camera::RenderSkybox()
{
	// If skybox is nullptr, get component
	if (m_pSkyBox == nullptr)
	{
		m_pSkyBox = GetComponent<DDM::SkyBoxComponent>();
	}

	// If skybox is not nullptr, render it
	if (m_pSkyBox != nullptr)
	{
		m_pSkyBox->RenderSkyBox();
	}
}

const glm::mat4& DDM::Camera::GetProjectionMatrix()
{
	// Return projection matrix
	return m_ProjectionMatrix;
}

glm::mat4* DDM::Camera::GetProjectionMatrixPtr()
{
	// Return pointer to projection matrix
	return &m_ProjectionMatrix;
}

const glm::mat4& DDM::Camera::GetViewMatrix()
{
	// Return view matrix
	return m_ViewMatrix;
}

glm::mat4* DDM::Camera::GetViewMatrixPtr()
{
	// Return pointer to view matrix
	return &m_ViewMatrix;
}

void DDM::Camera::UpdateViewMatrix()
{
	if (m_pTransform == nullptr)
	{
		m_pTransform = GetTransform();
	}

	// Create rotation matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(glm::conjugate(m_pTransform->GetWorldRotation()));

	// Create translation matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -m_pTransform->GetWorldPosition());

	// Multiply matrices (apply rotation first, then translation)
	m_ViewMatrix = rotationMatrix * translationMatrix;
}

void DDM::Camera::UpdateProjectionMatrix()
{
	// Get a reference to the window struct
	auto& windowStruct = Window::GetInstance().GetWindowStruct();

	// If width or height is 0, return
	if (windowStruct.Width == 0 || windowStruct.Height == 0)
	{
		return;
	}

	// Set the projection matrix
	m_ProjectionMatrix = glm::perspective(m_FovAngle, windowStruct.Width / static_cast<float>(windowStruct.Height), 0.001f, 100.0f);

	// Invert the Y axis to match Vulkan's coordinate system
	m_ProjectionMatrix[1][1] *= -1;
	
	// Invert the Z axis to match Vulkan's coordinate system
	m_ProjectionMatrix[2][2] *= -1;

	// Invert the Z axis to match Vulkan's coordinate system for left-handed systems
	m_ProjectionMatrix[2][3] *= -1;
}
