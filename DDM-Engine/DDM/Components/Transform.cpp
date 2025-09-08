// Transform.cpp

// Header include
#include "Transform.h"

// File includes
#include "Includes/GLFWIncludes.h"
#include "Includes/ImGuiIncludes.h"

// Standard library includes
#include <math.h>

#include <fstream>
#include <iostream>
#include <filesystem>

void DDM::Transform::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	// Start a tree node
	if (ImGui::TreeNodeEx("Transform", flags))
	{
		// Start a tree node for location
		if (ImGui::TreeNodeEx("Location", flags))
		{
			// Add sliders for all directions
			ImGui::SliderFloat("x", &m_LocalPosition.x, -10, 10);
			ImGui::SliderFloat("y", &m_LocalPosition.y, -10, 10);
			ImGui::SliderFloat("z", &m_LocalPosition.z, -10, 10);

			// End tree node
			ImGui::TreePop();
		}

		// Start tree node for rotation
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

			// End tree node
			ImGui::TreePop();
		}

		// Start a tree node for scale
		if (ImGui::TreeNodeEx("Scale", flags))
		{
			// Add slider for each direction
			ImGui::SliderFloat("x", &m_LocalScale.x, 0.001f, 2.0f);
			ImGui::SliderFloat("y", &m_LocalScale.y, 0.001f, 2.0f);
			ImGui::SliderFloat("z", &m_LocalScale.z, 0.001f, 2.0f);

			// End tree node
			ImGui::TreePop();
		}

		// End tree node
		ImGui::TreePop();
	}

	// Set the dirty flag
	SetPositionDirtyFlag();
	SetRotationDirtyFlag();
	SetScaleDirtyFlag();
}

void DDM::Transform::SetLocalPosition(float x, float y, float z)
{
	// Propagate to overloaded function
	SetLocalPosition(glm::vec3{ x, y, z });
}

void DDM::Transform::SetLocalPosition(const glm::vec3& pos)
{
	// Set new local position
	m_LocalPosition = pos;

	// Set dirty flag
	SetPositionDirtyFlag();
}

void DDM::Transform::SetLocalPosition(const glm::vec3&& pos)
{
	// Propagate to lvalue overloaded function
	SetLocalPosition(pos);
}

void DDM::Transform::Translate(float x, float y, float z)
{
	// Propagate to overloaded function
	Translate(glm::vec3{ x, y, z });
}

void DDM::Transform::Translate(const glm::vec3& dir)
{
	// Set new local position
	SetLocalPosition(m_LocalPosition + dir);
}

void DDM::Transform::Translate(const glm::vec3&& dir)
{
	// Propagate to lvalue overloaded function
	Translate(dir);
}

glm::vec3 DDM::Transform::GetWorldPosition()
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

void DDM::Transform::SetWorldPosition(float x, float y, float z)
{
	// Propagate to overloaded function
	SetWorldPosition(glm::vec3{ x, y, z });
}

void DDM::Transform::SetWorldPosition(const glm::vec3& pos)
{
	// Set new local position
	SetLocalPosition(pos - (GetWorldPosition() - m_LocalPosition));
}

void DDM::Transform::SetWorldPosition(const glm::vec3&& pos)
{
	// Propagate to lvalue overloaded function
	SetWorldPosition(pos);
}

void DDM::Transform::SetPositionDirtyFlag() const
{
	// Loop through all children
	for (auto& pChild : GetOwner()->GetChildren())
	{
		// Extract transform and set dirty flag
		auto transform = pChild->GetTransform();
		transform->m_PositionDF = true;

		// Propagate dirty flag through all children
		transform->SetPositionDirtyFlag();
	}
}

const glm::vec3 DDM::Transform::GetLocalRotationEuler() const
{
	// Convert rotation to euler angles and return
	return glm::eulerAngles(m_LocalRotation);
}

void DDM::Transform::SetLocalRotation(float x, float y, float z)
{
	// Propagate to overloaded function
	SetLocalRotation(glm::vec3{ x, y ,z });
}

void DDM::Transform::SetLocalRotation(const glm::vec3& rot)
{
	// Convert to quaternion and propagate to overloaded function
	SetLocalRotation(glm::quat(rot));
}

void DDM::Transform::SetLocalRotation(const glm::vec3&& rot)
{
	// Propagate to lvalue overloaded function
	SetLocalRotation(rot);
}

void DDM::Transform::SetLocalRotation(const glm::quat& rot)
{
	// Set local rotation
	m_LocalRotation = rot;

	// Set dirty flag for rotation
	SetRotationDirtyFlag();
}

void DDM::Transform::SetLocalRotation(const glm::quat&& rot)
{
	// Propagate to lvalue overloaded function
	SetLocalRotation(rot);
}

void DDM::Transform::Rotate(glm::vec3& axis, float angle)
{
	// Rotate local rotation around axiss
	m_LocalRotation = glm::rotate(m_LocalRotation, angle, axis);

	// Set dirty flag for rotation
	SetRotationDirtyFlag();
}

void DDM::Transform::Rotate(glm::vec3&& axis, float angle)
{
	// Propagate to lvalue overloaded function
	Rotate(axis, angle);
}

void DDM::Transform::RotateDeg(glm::vec3& axis, float angle)
{
	// Convert to radians and propagate
	Rotate(axis, glm::radians(angle));
}

void DDM::Transform::RotateDeg(glm::vec3&& axis, float angle)
{
	// Propagate to lvalue overloaded function
	RotateDeg(axis, angle);
}

glm::quat DDM::Transform::GetWorldRotation()
{
	// Calculate and return world rotation
	return GetParentRotation() * m_LocalRotation;
}

void DDM::Transform::SetWorldRotation(float x, float y, float z)
{
	// Propagate to overloaded function
	SetWorldRotation(glm::vec3{x, y, z});
}

void DDM::Transform::SetWorldRotation(const glm::vec3& rot)
{
	// Convert to quaternion
	glm::quat worldRotation = glm::quat(rot);

	// Propagate to overloaded function
	SetWorldRotation(worldRotation);
}

void DDM::Transform::SetWorldRotation(const glm::vec3&& rot)
{
	// Propagate to lvalue overloaded function
	SetWorldRotation(rot);
}

void DDM::Transform::SetWorldRotation(const glm::quat& rot)
{
	// Get the parent's world rotation
	glm::quat parentWorldRotation = GetParentRotation();

	// Adjust the local rotation to achieve the desired world rotation
	glm::quat localRotation = rot * glm::inverse(parentWorldRotation);

	// Set the local rotation
	SetLocalRotation(localRotation);
}

void DDM::Transform::SetWorldRotation(const glm::quat&& rot)
{
	// Propagate to lvalue overloaded function
	SetWorldRotation(rot);
}

void DDM::Transform::SetRotationDirtyFlag() const
{
	// Loop through all children, set dirty flag and propagate
	for (auto& pChild : GetOwner()->GetChildren())
	{
		auto transform = pChild->GetTransform();
		transform->m_RotationDF = true;
		transform->SetRotationDirtyFlag();
	}
}

void DDM::Transform::SetLocalScale(float x, float y, float z)
{
	// Propagate to overloaded function
	SetLocalScale(glm::vec3{ x, y, z });
}

void DDM::Transform::SetLocalScale(const glm::vec3& scale)
{
	// Set local scale
	m_LocalScale = scale;

	// Set dirty flag
	SetScaleDirtyFlag();
}

void DDM::Transform::SetLocalScale(const glm::vec3&& scale)
{
	// Propagate to lvalue overloaded function
	SetLocalScale(scale);
}

glm::vec3 DDM::Transform::GetWorldScale()
{
	return GetParentScale() * m_LocalScale;
}

void DDM::Transform::SetWorldScale(float x, float y, float z)
{

	// Propagate to overloaded function
	SetWorldScale(glm::vec3(x, y, z));
}

void DDM::Transform::SetWorldScale(const glm::vec3& scale)
{
	// Calculate new local scale and set it
	SetLocalScale(scale/(GetWorldScale()/m_LocalScale));
}

void DDM::Transform::SetWorldScale(const glm::vec3&& scale)
{
	// Propagate to lvalue overloaded function
	SetWorldScale(scale);
}

void DDM::Transform::SetScaleDirtyFlag() const
{
	// Loop through all children, set dirty flag and propagate
	for (auto& pChild : GetOwner()->GetChildren())
	{
		auto transform = pChild->GetTransform();
		transform->m_ScaleDF = true;
		transform->SetScaleDirtyFlag();
	}
}

glm::vec3 DDM::Transform::GetForward()
{
	// Create the rotation matrix from the quaternion
	glm::mat4 rotationMatrix = glm::mat4_cast(GetWorldRotation());

	// Apply the rotation to the vector using the rotation matrix
	glm::vec4 rotatedVector = rotationMatrix * glm::vec4(0.f, 0.f, 1.f, 0.0f);  // Use (0, 0, 1) for the forward vector

	// Extract the rotated glm::vec3 from the glm::vec4
	glm::vec3 finalRotatedVector = glm::vec3(rotatedVector);

	return finalRotatedVector;
}

glm::vec3 DDM::Transform::GetUp()
{
	// Create the rotation matrix from the quaternion
	glm::mat4 rotationMatrix = glm::mat4_cast(GetWorldRotation());

	// Apply the rotation to the vector using the rotation matrix
	glm::vec4 rotatedVector = rotationMatrix * glm::vec4(0.f, 1.f, 0.f, 0.0f);  // Use (0, 1, 0) for the up vector

	// Extract the rotated glm::vec3 from the glm::vec4
	glm::vec3 finalRotatedVector = glm::vec3(rotatedVector);

	return finalRotatedVector;
}

glm::vec3 DDM::Transform::GetRight()
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

bool DDM::Transform::WriteToFile(std::string& fileName)
{
	// Get the index of the final period in the name, all characters after it indicate the extension
	auto index = fileName.find_last_of("/");

	auto directoryName = fileName.substr(0, index);

	std::filesystem::create_directory(directoryName);


	// Set up POD
	TransformPod toWrite{};
	toWrite.pos = GetWorldPosition();
	toWrite.rot = GetWorldRotation();
	toWrite.scale = GetWorldScale();

	// Open the file
	std::ofstream temp;
	temp.open(fileName, std::ios::binary | std::ios::out);

	// If file is open, read from it
	if (temp.is_open())
	{
		temp.write((const char*)&toWrite, sizeof(toWrite));
		temp.close();

		// Indicate that writing was successful
		return true;
	}

	// Indicate that writing failed
	return false;
}

bool DDM::Transform::WriteToFile(std::string&& fileName)
{
	// Propagate to overloaded lvalue function
	return WriteToFile(fileName);
}

bool DDM::Transform::ReadFromFile(std::string& fileName)
{
	// Open file
	std::ifstream input;
	input.open(fileName, std::ios::binary);
	
	// If file is open, read from it
	if (input.is_open())
	{
		// Set up POD
		TransformPod toRead{};

		// Read and close file
		input.read((char*)&toRead, sizeof(toRead));
		input.close();

		// Set all values correctly
		SetWorldPosition(toRead.pos);
		SetWorldRotation(toRead.rot);
		SetWorldScale(toRead.scale);

		// Indicate that reading was successful
		return true;
	}

	// Indicate that reading failed
	return false;
}

bool DDM::Transform::ReadFromFile(std::string&& fileName)
{
	// Propagate to overloaded lvalue function
	return ReadFromFile(fileName);
}

const glm::vec3& DDM::Transform::GetParentPosition()
{
	// If no owner or parent, set parent world position to 0,0,0
	if (GetOwner() == nullptr || GetOwner()->GetParent() == nullptr)
	{
		m_ParentWorldPosition = glm::vec3{};
		m_PositionDF = false;
	}
	// If Dirty flag is set, save new parent position
	else if (m_PositionDF)
	{
		m_ParentWorldPosition = GetOwner()->GetParent()->GetTransform()->GetWorldPosition();
		m_PositionDF = false;
	}

	// return parent position
	return m_ParentWorldPosition;
}

const glm::quat& DDM::Transform::GetParentRotation()
{
	// If no owner or parent, set parent world rotation to identity quaternion
	if (GetOwner() == nullptr || GetOwner()->GetParent() == nullptr)
	{
		m_ParentWorldRotation = glm::identity<glm::quat>();;
		m_RotationDF = false;
	}
	// If dirty flag is set, save new parent rotation
	else if (m_RotationDF)
	{
		m_ParentWorldRotation = GetOwner()->GetParent()->GetTransform()->GetWorldRotation();
		m_RotationDF = false;
	}

	// Return parent rotation
	return m_ParentWorldRotation;
}

const glm::vec3& DDM::Transform::GetParentScale()
{
	// If no owner or parent, set parent world scale to 1,1,1
	if (GetOwner() == nullptr || GetOwner()->GetParent() == nullptr)
	{
		m_ParentWorldScale = glm::vec3{ 1, 1, 1 };
		m_ScaleDF = false;
	}
	// If dirty flag is set, save new parent scale
	else if (m_ScaleDF)
	{
		m_ParentWorldScale = GetOwner()->GetParent()->GetTransform()->GetWorldScale();
		m_ScaleDF = false;
	}

	// Return parent scale
	return m_ParentWorldScale;
}
