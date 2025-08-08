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

	static bool keyPressedLastFrame = false;

	bool keyPressed = glfwGetKey(Window::GetInstance().GetWindowStruct().pWindow, GLFW_KEY_P) == GLFW_PRESS;

	if (keyPressed && !keyPressedLastFrame)
	{
		PrintTransform();
	}

	keyPressedLastFrame = keyPressed;
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

	if (windowStruct.Width == 0 || windowStruct.Height == 0)
	{
		return;
	}

	// Set the projection matrix
	m_ProjectionMatrix = glm::perspective(m_FovAngle, windowStruct.Width / static_cast<float>(windowStruct.Height), 0.001f, 100.0f);

	m_ProjectionMatrix[1][1] *= -1;
	m_ProjectionMatrix[2][2] *= -1;
	m_ProjectionMatrix[2][3] *= -1;
}

void DDM3::CameraComponent::PrintTransform()
{
	auto transform = GetTransform();

	auto pos = transform->GetWorldPosition();

	auto rot = glm::eulerAngles(transform->GetWorldRotation());

	std::cout << "Camera transform: " << std::endl;
	std::cout << "Location: " << std::endl;
	std::cout << "X: " << pos.x << ", Y: " << pos.y << ", Z: " << pos.z << std::endl;
	std::cout << "Rotation: " << std::endl;
	std::cout << "X: " << rot.x << ", Y: " << rot.y << ", Z: " << rot.z << std::endl;

}
