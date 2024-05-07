#include "DirectionalLightComponent.h"
#include "TransformComponent.h"

#include "../Includes/ImGuiIncludes.h"


D3D::DirectionalLightComponent::DirectionalLightComponent()
{
	CreateLightBuffer();

	/*for (int frame{}; frame < VulkanRenderer::GetInstance().GetMaxFrames(); frame++)
	{
		UpdateBuffer(frame);
	}*/
}

void D3D::DirectionalLightComponent::OnGUI()
{
	static bool test{ true };

	ImGui::Begin(GetOwner()->GetName().c_str(), &test);

	ImGuiTreeNodeFlags mainFlags = ImGuiTreeNodeFlags_Framed;

	if (ImGui::TreeNodeEx("General Info", mainFlags))
	{
		std::string text = "x: " + std::to_string(m_BufferObject.direction.x) +
			"y: " + std::to_string(m_BufferObject.direction.y) +
			"z: " + std::to_string(m_BufferObject.direction.z);

		ImGui::Text(text.c_str());

		ImGui::TreePop();
	}


	ImGui::End();
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
