// ModelLoader.cpp

// Header include
#include "ModelLoader.h"

// File includes
#include "Includes/ImGuiIncludes.h"
#include "Managers/SceneManager.h"
#include "Managers/ResourceManager.h"

#include "Engine/Window.h"
#include "Engine/Scene.h"

#include "DataTypes/Mesh.h"
#include "DataTypes/Materials/MultiMaterial.h"

#include "Components/MeshRenderer.h"
#include "Components/TransformComponent.h"

// Standard library includes
#include <iostream>
#include <algorithm>

DDM::ModelLoader::ModelLoader()
	:Component()
{
	// Add callback to window
	auto boundCallback = std::bind(&DDM::ModelLoader::DropFileCallback, this, std::placeholders::_1, std::placeholders::_2);
	Window::GetInstance().AddCallback(this, boundCallback);
}

DDM::ModelLoader::~ModelLoader()
{
	// Remove callback
	Window::GetInstance().RemoveCallback(this);
}

void DDM::ModelLoader::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	// Start tree
	if (ImGui::TreeNodeEx("Model Loader", flags))
	{
		// Create input text for object name and file path
		ImGui::InputText("Object name", m_ObjectName, IM_ARRAYSIZE(m_ObjectName));
		ImGui::InputText("File path", m_FilePath, IM_ARRAYSIZE(m_FilePath));

		// Check if inputtext for filepath is hovered
		m_IsTextboxHovered = ImGui::IsItemHovered();
		
		// Create a button
		if (ImGui::Button("Submit"))
		{
			// Call the function with the text input
			LoadObject();
		}

		// Close tree
		ImGui::TreePop();
	}
}

void DDM::ModelLoader::LoadObject()
{
	// If no object name, return
	if (m_ObjectName[0] == '\0')
		return;

	// Create a new game object with the given name
	auto pObject{ SceneManager::GetInstance().GetActiveScene()->CreateGameObject(m_ObjectName)};
	pObject->SetShowImGui(true);

	// Create a new multimaterial and load the mesh
	std::shared_ptr<DDM::MultiMaterial> pMaterial{ std::make_shared<DDM::MultiMaterial>() };
	auto pMesh{ DDM::ResourceManager::GetInstance().LoadMesh(m_FilePath) };

	// Add a mesh renderer component to the object and set the mesh and material
	auto pMeshRenderer{ pObject->AddComponent<DDM::MeshRenderComponent>() };
	pMeshRenderer->SetShowImGui(true);
	pMeshRenderer->SetMesh(pMesh);
	pMeshRenderer->SetMaterial(pMaterial);

	// Set the transform component to show ImGui
	auto pMeshTransform{ pObject->GetTransform() };
	pMeshTransform->SetShowImGui(true);
	
}

void DDM::ModelLoader::DropFileCallback(int count, const char** paths)
{
	// Check if textbox is hovered
	if (!m_IsTextboxHovered || count <= 0)
		return;

	// Clear the filepath
	std::fill(std::begin(m_FilePath), std::end(m_FilePath), 0);

	// Copy the first path to the filepath
	auto filePath = paths[0];

	for (int i{}; i < 125; i++)
	{
		// If end of string, break
		if (filePath[i] == '\0')
		{
			break;
		}
		else
		{
			m_FilePath[i] = filePath[i];
		}
	}
}
