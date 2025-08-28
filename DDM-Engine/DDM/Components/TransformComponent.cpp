#include "TransformComponent.h"
#include "../Includes/GLFWIncludes.h"
#include "../Includes/ImGuiIncludes.h"

#include <math.h>

void DDM::TransformComponent::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	if (ImGui::TreeNodeEx("Transform", flags))
	{

		if (ImGui::TreeNodeEx("Location", flags))
		{
			ImGui::SliderFloat("x", &m_LocalPosition.x, -10, 10);
			ImGui::SliderFloat("y", &m_LocalPosition.y, -10, 10);
			ImGui::SliderFloat("z", &m_LocalPosition.z, -10, 10);
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Rotation", flags))
		{
			// Convert quaternion to Euler angles in degrees
			glm::vec3 rotation =glm::eulerAngles(m_LocalRotation);

			// Create sliders for each axis
			ImGui::SliderAngle("x", &rotation.x);
			ImGui::SliderAngle("y", &rotation.y);
			ImGui::SliderAngle("z", &rotation.z);

			// Update quaternion with new Euler angles
			m_LocalRotation = glm::quat(rotation);

			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Scale", flags))
		{
			ImGui::SliderFloat("x", &m_LocalScale.x, 0.001f, 2.0f);
			ImGui::SliderFloat("y", &m_LocalScale.y, 0.001f, 2.0f);
			ImGui::SliderFloat("z", &m_LocalScale.z, 0.001f, 2.0f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	SetPositionDirtyFlag();
	SetRotationDirtyFlag();
	SetScaleDirtyFlag();
}

void DDM::TransformComponent::SetLocalPosition(float x, float y, float z)
{
	SetLocalPosition(glm::vec3{ x, y, z });
}

void DDM::TransformComponent::SetLocalPosition(const glm::vec3& pos)
{
	m_LocalPosition = pos;

	SetPositionDirtyFlag();
}

void DDM::TransformComponent::Translate(float x, float y, float z)
{
	Translate(glm::vec3{ x, y, z });
}

void DDM::TransformComponent::Translate(const glm::vec3& dir)
{
	SetLocalPosition(m_LocalPosition + dir);
}

glm::vec3 DDM::TransformComponent::GetWorldPosition()
{
	// Get the parent's position
	glm::vec3 parentPosition = GetParentPosition();

	// Create the rotation matrix from the quaternion
	glm::mat4 rotationMatrix = glm::mat4_cast(GetParentRotation());

	// Apply the rotation to the vector using the rotation matrix
	glm::vec4 rotatedVector = rotationMatrix * glm::vec4(m_LocalPosition, 1.0f);

	// Extract the rotated glm::vec3 from the glm::vec4
	glm::vec3 finalRotatedVector = glm::vec3(rotatedVector);

	// Calculate the final position after rotation
	glm::vec3 finalPosition = parentPosition + finalRotatedVector;

	return finalPosition;
}

void DDM::TransformComponent::SetWorldPosition(float x, float y, float z)
{
	SetWorldPosition(glm::vec3{ x, y, z });
}

void DDM::TransformComponent::SetWorldPosition(const glm::vec3& pos)
{
	SetLocalPosition(pos - (GetWorldPosition() - m_LocalPosition));
}

void DDM::TransformComponent::SetPositionDirtyFlag() const
{
	for (auto& pChild : GetOwner()->GetChildren())
	{
		auto transform = pChild->GetTransform();
		transform->m_PositionDF = true;
		transform->SetPositionDirtyFlag();
	}
}

void DDM::TransformComponent::SetLocalRotation(float x, float y, float z)
{
	SetLocalRotation(glm::vec3{ x, y ,z });
}

void DDM::TransformComponent::SetLocalRotation(const glm::vec3& rot)
{
	m_LocalRotation = glm::quat(rot);

	SetRotationDirtyFlag();
}

void DDM::TransformComponent::SetLocalRotation(const glm::vec3&& rot)
{
	SetLocalRotation(rot);
}

void DDM::TransformComponent::SetLocalRotation(const glm::quat& rot)
{
	m_LocalRotation = rot;

	SetRotationDirtyFlag();
}

void DDM::TransformComponent::SetLocalRotation(const glm::quat&& rot)
{
	SetLocalRotation(rot);
}

void DDM::TransformComponent::Rotate(glm::vec3& axis, float angle)
{
	m_LocalRotation = glm::rotate(m_LocalRotation, angle, axis);

	SetRotationDirtyFlag();
}

void DDM::TransformComponent::Rotate(glm::vec3&& axis, float angle)
{
	Rotate(axis, angle);
}

glm::quat DDM::TransformComponent::GetWorldRotation()
{
	return GetParentRotation() * m_LocalRotation;
}

void DDM::TransformComponent::SetWorldRotation(float x, float y, float z)
{
	SetWorldRotation(glm::vec3{x, y, z});
}

void DDM::TransformComponent::SetWorldRotation(const glm::vec3& rot)
{
	// Calculate the rotation needed to achieve the desired world rotation
	glm::quat worldRotation = glm::quat(rot);

	// Get the parent's world rotation
	glm::quat parentWorldRotation = GetParentRotation();

	// Adjust the local rotation to achieve the desired world rotation
	glm::quat localRotation =  worldRotation * glm::inverse(parentWorldRotation);

	// Set the local rotation
	SetLocalRotation(localRotation);
}

void DDM::TransformComponent::SetRotationDirtyFlag() const
{
	for (auto& pChild : GetOwner()->GetChildren())
	{
		auto transform = pChild->GetTransform();
		transform->m_RotationDF = true;
		transform->SetRotationDirtyFlag();
	}
}

void DDM::TransformComponent::SetLocalScale(float x, float y, float z)
{
	SetLocalScale(glm::vec3{ x, y, z });
}

void DDM::TransformComponent::SetLocalScale(const glm::vec3& scale)
{
	m_LocalScale = scale;

	SetScaleDirtyFlag();
}

glm::vec3 DDM::TransformComponent::GetWorldScale()
{
	return GetParentScale() * m_LocalScale;
}

void DDM::TransformComponent::SetWorldScale(float x, float y, float z)
{
	SetWorldScale(glm::vec3(x, y, z));
}

void DDM::TransformComponent::SetWorldScale(const glm::vec3 scale)
{
	SetLocalScale(scale/(GetWorldScale()/m_LocalScale));
}

void DDM::TransformComponent::SetScaleDirtyFlag() const
{
	for (auto& pChild : GetOwner()->GetChildren())
	{
		auto transform = pChild->GetTransform();
		transform->m_ScaleDF = true;
		transform->SetScaleDirtyFlag();
	}
}

glm::vec3 DDM::TransformComponent::GetForward()
{
	// Create the rotation matrix from the quaternion
	glm::mat4 rotationMatrix = glm::mat4_cast(GetWorldRotation());

	// Apply the rotation to the vector using the rotation matrix
	glm::vec4 rotatedVector = rotationMatrix * glm::vec4(0.f, 0.f, 1.f, 0.0f);

	// Extract the rotated glm::vec3 from the glm::vec4
	glm::vec3 finalRotatedVector = glm::vec3(rotatedVector);

	return finalRotatedVector;
}

glm::vec3 DDM::TransformComponent::GetUp()
{
	// Create the rotation matrix from the quaternion
	glm::mat4 rotationMatrix = glm::mat4_cast(GetWorldRotation());

	// Apply the rotation to the vector using the rotation matrix
	glm::vec4 rotatedVector = rotationMatrix * glm::vec4(0.f, 1.f, 0.f, 0.0f);  // Use (0, 1, 0) for the up vector

	// Extract the rotated glm::vec3 from the glm::vec4
	glm::vec3 finalRotatedVector = glm::vec3(rotatedVector);

	return finalRotatedVector;
}

glm::vec3 DDM::TransformComponent::GetRight()
{
	// Create the rotation matrix from the quaternion
	glm::mat4 rotationMatrix = glm::mat4_cast(GetWorldRotation());

	// Apply the rotation to the vector using the rotation matrix
	glm::vec4 rotatedVector = rotationMatrix * glm::vec4(1.f, 0.f, 0.f, 0.0f);  // Use (1, 0, 0) for the right vector

	// Extract the rotated glm::vec3 from the glm::vec4
	glm::vec3 finalRotatedVector = glm::vec3(rotatedVector);

	finalRotatedVector.y *= -1;

	return finalRotatedVector;
}

glm::vec3& DDM::TransformComponent::GetParentPosition()
{
	if (GetOwner() == nullptr || GetOwner()->GetParent() == nullptr)
	{
		m_ParentWorldPosition = glm::vec3{};
		m_PositionDF = false;
	}
	else if (m_PositionDF)
	{
		m_ParentWorldPosition = GetOwner()->GetParent()->GetTransform()->GetWorldPosition();
		m_PositionDF = false;
	}

	return m_ParentWorldPosition;
}

glm::quat& DDM::TransformComponent::GetParentRotation()
{
	if (GetOwner() == nullptr || GetOwner()->GetParent() == nullptr)
	{
		m_ParentWorldRotation = glm::identity<glm::quat>();;
		m_RotationDF = false;
	}
	else if (m_RotationDF)
	{
		m_ParentWorldRotation = GetOwner()->GetParent()->GetTransform()->GetWorldRotation();
		m_RotationDF = false;
	}

	return m_ParentWorldRotation;
}

glm::vec3& DDM::TransformComponent::GetParentScale()
{
	if (GetOwner() == nullptr || GetOwner()->GetParent() == nullptr)
	{
		m_ParentWorldScale = glm::vec3{ 1, 1, 1 };
		m_ScaleDF = false;
	}
	else if (m_ScaleDF)
	{
		m_ParentWorldScale = GetOwner()->GetParent()->GetTransform()->GetWorldScale();
		m_ScaleDF = false;
	}

	return m_ParentWorldScale;
}
