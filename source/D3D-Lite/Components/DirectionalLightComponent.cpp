#include "DirectionalLightComponent.h"
#include "TransformComponent.h"

#include "../Includes/ImGuiIncludes.h"


D3D::DirectionalLightComponent::DirectionalLightComponent()
{
	CreateLightBuffer();
}

void D3D::DirectionalLightComponent::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	if (ImGui::TreeNodeEx("Light", flags))
	{
		ImGui::ColorEdit3("Color", glm::value_ptr(m_BufferObject.color));

		ImGui::SliderFloat("Intensity", &m_BufferObject.intensity, 0, 1);
		
		ImGui::TreePop();
	}
}

void D3D::DirectionalLightComponent::LateUpdate()
{
	UpdateBuffer(D3D::VulkanRenderer::GetInstance().GetCurrentFrame());
}

void D3D::DirectionalLightComponent::SetColor(glm::vec3& color)
{
	m_BufferObject.color = color;
}

void D3D::DirectionalLightComponent::SetColor(glm::vec3&& color)
{
	m_BufferObject.color = color;
}

void D3D::DirectionalLightComponent::SetIntensity(float intensity)
{
	m_BufferObject.intensity = intensity;
}

D3D::DescriptorObject* D3D::DirectionalLightComponent::GetDescriptorObject()
{
	return static_cast<DescriptorObject*>(m_DescriptorObject.get());
}

void D3D::DirectionalLightComponent::CreateLightBuffer()
{
	m_DescriptorObject = std::make_unique<UboDescriptorObject<DirectionalLightStruct>>();
}

void D3D::DirectionalLightComponent::UpdateBuffer(int frame)
{
	m_BufferObject.direction = GetTransform()->GetForward();
	m_DescriptorObject->UpdateUboBuffer(m_BufferObject, frame);
}
