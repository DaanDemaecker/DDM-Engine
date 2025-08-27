#include "DirectionalLightComponent.h"
#include "TransformComponent.h"

#include "../Includes/ImGuiIncludes.h"


DDM::DirectionalLightComponent::DirectionalLightComponent()
{
	CreateLightBuffer();
}

void DDM::DirectionalLightComponent::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	if (ImGui::TreeNodeEx("Light", flags))
	{
		ImGui::ColorEdit3("Color", glm::value_ptr(m_BufferObject.color));

		ImGui::SliderFloat("Intensity", &m_BufferObject.intensity, 0, 1);
		
		ImGui::TreePop();
	}
}

void DDM::DirectionalLightComponent::LateUpdate()
{
	UpdateBuffer(DDM::VulkanObject::GetInstance().GetCurrentFrame());
}

void DDM::DirectionalLightComponent::SetColor(glm::vec3& color)
{
	m_BufferObject.color = color;
}

void DDM::DirectionalLightComponent::SetColor(glm::vec3&& color)
{
	m_BufferObject.color = color;
}

void DDM::DirectionalLightComponent::SetIntensity(float intensity)
{
	m_BufferObject.intensity = intensity;
}

DDM::DescriptorObject* DDM::DirectionalLightComponent::GetDescriptorObject()
{
	return static_cast<DescriptorObject*>(m_DescriptorObject.get());
}

void DDM::DirectionalLightComponent::CreateLightBuffer()
{
	m_DescriptorObject = std::make_unique<UboDescriptorObject<DirectionalLightStruct>>();
}

void DDM::DirectionalLightComponent::UpdateBuffer(int frame)
{
	m_BufferObject.direction = GetTransform()->GetForward();
	m_DescriptorObject->UpdateUboBuffer(&m_BufferObject, frame);
}
