#include "TransformComponent.h"
#include "GLMIncludes.h"
#include "ImGuiIncludes.h"

#include <math.h>

void D3D::TransformComponent::OnGUI()
{
	static bool test{ true };

	ImGui::Begin(GetOwner()->GetName().c_str(), &test);


	auto vector{ GetWorldPosition() };

		std::string text = "x: " + std::to_string(vector.x) +
			"y: " + std::to_string(vector.y) +
			"z: " + std::to_string(vector.z);

		ImGui::Text("World position: ");
		ImGui::Text(text.c_str());


		vector = GetForward();
		text = "x: " + std::to_string(vector.x) +
			"y: " + std::to_string(vector.y) +
			"z: " + std::to_string(vector.z);

		ImGui::Text("Forward: ");
		ImGui::Text(text.c_str());

		vector = GetRight();
		text = "x: " + std::to_string(vector.x) +
			"y: " + std::to_string(vector.y) +
			"z: " + std::to_string(vector.z);

		ImGui::Text("Right: ");
		ImGui::Text(text.c_str());

		vector = GetUp();
		text = "x: " + std::to_string(vector.x) +
			"y: " + std::to_string(vector.y) +
			"z: " + std::to_string(vector.z);

		ImGui::Text("Up: ");
		ImGui::Text(text.c_str());


	ImGui::End();
}

void D3D::TransformComponent::SetLocalPosition(float x, float y, float z)
{
	SetLocalPosition(glm::vec3{ x, y, z });
}

void D3D::TransformComponent::SetLocalPosition(const glm::vec3& pos)
{
	m_LocalPosition = pos;

	SetPositionDirtyFlag();
}

void D3D::TransformComponent::Translate(float x, float y, float z)
{
	Translate(glm::vec3{ x, y, z });
}

void D3D::TransformComponent::Translate(const glm::vec3& dir)
{
	SetLocalPosition(m_LocalPosition + dir);
}

glm::vec3 D3D::TransformComponent::GetWorldPosition()
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

void D3D::TransformComponent::SetWorldPosition(float x, float y, float z)
{
	SetWorldPosition(glm::vec3{ x, y, z });
}

void D3D::TransformComponent::SetWorldPosition(const glm::vec3& pos)
{
	SetLocalPosition(pos - (GetWorldPosition() - m_LocalPosition));
}

void D3D::TransformComponent::SetPositionDirtyFlag()
{
	for (auto& pChild : GetOwner()->GetChildren())
	{
		auto transform = pChild->GetTransform();
		transform->m_PositionDF = true;
		transform->SetPositionDirtyFlag();
	}
}

void D3D::TransformComponent::SetLocalRotation(float x, float y, float z)
{
	SetLocalRotation(glm::vec3{ x, y ,z });
}

void D3D::TransformComponent::SetLocalRotation(const glm::vec3& rot)
{
	m_LocalRotation = glm::quat(rot);

	SetRotationDirtyFlag();
}

void D3D::TransformComponent::SetLocalRotation(const glm::vec3&& rot)
{
	SetLocalRotation(rot);
}

void D3D::TransformComponent::SetLocalRotation(const glm::quat& rot)
{
	m_LocalRotation = rot;

	SetRotationDirtyFlag();
}

void D3D::TransformComponent::SetLocalRotation(const glm::quat&& rot)
{
	SetLocalRotation(rot);
}

void D3D::TransformComponent::Rotate(glm::vec3& axis, float angle)
{
	m_LocalRotation = glm::rotate(m_LocalRotation, angle, axis);

	SetRotationDirtyFlag();
}

void D3D::TransformComponent::Rotate(glm::vec3&& axis, float angle)
{
	Rotate(axis, angle);
}

glm::quat D3D::TransformComponent::GetWorldRotation()
{
	return GetParentRotation() * m_LocalRotation;
}

void D3D::TransformComponent::SetWorldRotation(float x, float y, float z)
{
	SetWorldRotation(glm::vec3{x, y, z});
}

void D3D::TransformComponent::SetWorldRotation(const glm::vec3& rot)
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

void D3D::TransformComponent::SetRotationDirtyFlag()
{
	for (auto& pChild : GetOwner()->GetChildren())
	{
		auto transform = pChild->GetTransform();
		transform->m_RotationDF = true;
		transform->SetRotationDirtyFlag();
	}
}

void D3D::TransformComponent::SetLocalScale(float x, float y, float z)
{
	SetLocalScale(glm::vec3{ x, y, z });
}

void D3D::TransformComponent::SetLocalScale(const glm::vec3& scale)
{
	m_LocalScale = scale;

	SetScaleDirtyFlag();
}

glm::vec3 D3D::TransformComponent::GetWorldScale()
{
	return GetParentScale() * m_LocalScale;
}

void D3D::TransformComponent::SetWorldScale(float x, float y, float z)
{
	SetWorldScale(glm::vec3(x, y, z));
}

void D3D::TransformComponent::SetWorldScale(const glm::vec3 scale)
{
	SetLocalScale(scale/(GetWorldScale()/m_LocalScale));
}

void D3D::TransformComponent::SetScaleDirtyFlag()
{
	for (auto& pChild : GetOwner()->GetChildren())
	{
		auto transform = pChild->GetTransform();
		transform->m_ScaleDF = true;
		transform->SetScaleDirtyFlag();
	}
}

glm::vec3 D3D::TransformComponent::GetForward()
{
	// Create the rotation matrix from the quaternion
	glm::mat4 rotationMatrix = glm::mat4_cast(GetWorldRotation());

	// Apply the rotation to the vector using the rotation matrix
	glm::vec4 rotatedVector = rotationMatrix * glm::vec4(0.f, 0.f, 1.f, 0.0f);

	// Extract the rotated glm::vec3 from the glm::vec4
	glm::vec3 finalRotatedVector = glm::vec3(rotatedVector);

	return finalRotatedVector;
}

glm::vec3 D3D::TransformComponent::GetUp()
{
	// Create the rotation matrix from the quaternion
	glm::mat4 rotationMatrix = glm::mat4_cast(GetWorldRotation());

	// Apply the rotation to the vector using the rotation matrix
	glm::vec4 rotatedVector = rotationMatrix * glm::vec4(0.f, 1.f, 0.f, 0.0f);  // Use (0, 1, 0) for the up vector

	// Extract the rotated glm::vec3 from the glm::vec4
	glm::vec3 finalRotatedVector = glm::vec3(rotatedVector);

	return finalRotatedVector;
}

glm::vec3 D3D::TransformComponent::GetRight()
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

glm::vec3& D3D::TransformComponent::GetParentPosition()
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

glm::quat& D3D::TransformComponent::GetParentRotation()
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

glm::vec3& D3D::TransformComponent::GetParentScale()
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
