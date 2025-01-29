#include "DirectionalLightComponent.h"
#include "TransformComponent.h"

#include "../Includes/ImGuiIncludes.h"


DDM3::DirectionalLightComponent::DirectionalLightComponent()
{
	CreateLightBuffer();
}

void DDM3::DirectionalLightComponent::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	if (ImGui::TreeNodeEx("Light", flags))
	{
		ImGui::ColorEdit3("Color", glm::value_ptr(m_BufferObject.color));

		ImGui::SliderFloat("Intensity", &m_BufferObject.intensity, 0, 1);
		
		ImGui::TreePop();
	}
}

void DDM3::DirectionalLightComponent::LateUpdate()
{
	UpdateBuffer(DDM3::VulkanRenderer::GetInstance().GetCurrentFrame());
}

void DDM3::DirectionalLightComponent::SetColor(glm::vec3& color)
{
	m_BufferObject.color = color;
}

void DDM3::DirectionalLightComponent::SetColor(glm::vec3&& color)
{
	m_BufferObject.color = color;
}

void DDM3::DirectionalLightComponent::SetIntensity(float intensity)
{
	m_BufferObject.intensity = intensity;
}

DDM3::DescriptorObject* DDM3::DirectionalLightComponent::GetDescriptorObject()
{
	return static_cast<DescriptorObject*>(m_DescriptorObject.get());
}

void DDM3::DirectionalLightComponent::CreateLightBuffer()
{
	m_DescriptorObject = std::make_unique<UboDescriptorObject<DirectionalLightStruct>>();
}

void DDM3::DirectionalLightComponent::UpdateBuffer(int frame)
{
	m_BufferObject.direction = GetTransform()->GetForward();
	m_DescriptorObject->UpdateUboBuffer(m_BufferObject, frame);
}
