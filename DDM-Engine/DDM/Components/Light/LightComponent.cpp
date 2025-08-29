// LightComponent.cpp

// Header include
#include "LightComponent.h"

// File includes
#include "Components/TransformComponent.h"
#include "Includes/GLMIncludes.h"
#include "Includes/ImGuiIncludes.h"


DDM::LightComponent::LightComponent()
{
	// Create the descriptor object
	m_DescriptorObject = std::make_unique<UboDescriptorObject<Light>>();
}

void DDM::LightComponent::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	// Start a tree
	if (ImGui::TreeNodeEx("Light", flags))
	{
		// Listbox for light type
		ImGui::ListBox("Type", reinterpret_cast<int*>(&m_BufferObject.type), m_LightTypeStrings, 3);

		// Slider for intensity
		ImGui::SliderFloat("Intensity", &m_BufferObject.intensity, 0, 1);

		// Slider for range
		ImGui::SliderFloat("Range", &m_BufferObject.range, 0.1f, 100.0f);

		// Slider for angle
		ImGui::SliderAngle("Angle", &m_BufferObject.angle, 1.0f, 179.0f);

		// Color edit for color
		ImGui::ColorEdit3("Color", glm::value_ptr(m_BufferObject.color));
		
		// End the tree
		ImGui::TreePop();
	}
}

void DDM::LightComponent::LateUpdate()
{
	// Update the buffer object
	UpdateBuffer(DDM::VulkanObject::GetInstance().GetCurrentFrame());
}

void DDM::LightComponent::SetColor(glm::vec3& color)
{
	// Set the color
	m_BufferObject.color = color;
}

void DDM::LightComponent::SetColor(glm::vec3&& color)
{
	// Propagate to lvalue overload
	SetColor(color);
}

void DDM::LightComponent::SetIntensity(float intensity)
{	
	// Set the intensity
	m_BufferObject.intensity = intensity;
}

void DDM::LightComponent::SetType(LightType type)
{
	// Set the type
	m_BufferObject.type = type;
}

void DDM::LightComponent::SetAngle(float angle)
{
	// Set the angle
	m_BufferObject.angle = angle;
}

void DDM::LightComponent::SetAngleDeg(float angle)
{
	// Propagate to radian overload
	SetAngle(glm::radians(angle));
}

void DDM::LightComponent::SetRange(float range)
{
	// Set the ragne
	m_BufferObject.range = range;
}

DDM::DescriptorObject* DDM::LightComponent::GetDescriptorObject()
{
	// Return pointer to descriptor object
	return static_cast<DescriptorObject*>(m_DescriptorObject.get());
}

void DDM::LightComponent::UpdateBuffer(int frame)
{
	// Fetch transform component
	auto transform = GetTransform();

	// Update direction and position
	m_BufferObject.direction = transform->GetForward();
	m_BufferObject.position = transform->GetWorldPosition();

	// Update the ubo buffer
	m_DescriptorObject->UpdateUboBuffer(&m_BufferObject, frame);
}
