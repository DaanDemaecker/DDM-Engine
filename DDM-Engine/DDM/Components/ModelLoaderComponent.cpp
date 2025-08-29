// ModelLoaderComponent.cpp

// File includes
#include "ModelLoaderComponent.h"
#include "../Includes/ImGuiIncludes.h"
#include "../Managers/SceneManager.h"
#include "../Engine/Scene.h"
#include "../DataTypes/Mesh.h"
#include "../DataTypes/Materials/MultiMaterial.h"
#include "Components/MeshRenderer.h"
#include "../Components/TransformComponent.h"
#include "../Engine/Window.h"
#include "../Managers/ResourceManager.h"

// Standard library includes
#include <iostream>
#include <algorithm>

DDM::ModelLoaderComponent::ModelLoaderComponent()
	:Component()
{
	auto boundCallback = std::bind(&DDM::ModelLoaderComponent::DropFileCallback, this, std::placeholders::_1, std::placeholders::_2);


	Window::GetInstance().AddCallback(this, boundCallback);
}

DDM::ModelLoaderComponent::~ModelLoaderComponent()
{
	Window::GetInstance().RemoveCallback(this);
}

void DDM::ModelLoaderComponent::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	if (ImGui::TreeNodeEx("Model Loader", flags))
	{
		ImGui::InputText("Object name", m_ObjectName, IM_ARRAYSIZE(m_ObjectName));
		ImGui::InputText("File path", m_FilePath, IM_ARRAYSIZE(m_FilePath));

		if (ImGui::IsItemHovered())
		{
			m_IsTextboxHovered = true;
		}
		else
		{
			m_IsTextboxHovered = false;
		}
		
		// Create a button
		if (ImGui::Button("Submit"))
		{
			// Call the function with the text input
			LoadObject();
		}

		ImGui::TreePop();
	}
}

void DDM::ModelLoaderComponent::LoadObject()
{
	if (m_ObjectName[0] == '\0')
		return;

	auto pObject{ SceneManager::GetInstance().GetActiveScene()->CreateGameObject(m_ObjectName)};
	pObject->SetShowImGui(true);

	std::shared_ptr<DDM::MultiMaterial> pMaterial{ std::make_shared<DDM::MultiMaterial>() };
	auto pMesh{ DDM::ResourceManager::GetInstance().LoadMesh(m_FilePath) };

	auto pMeshRenderer{ pObject->AddComponent<DDM::MeshRenderComponent>() };
	pMeshRenderer->SetShowImGui(true);
	pMeshRenderer->SetMesh(pMesh);
	pMeshRenderer->SetMaterial(pMaterial);

	auto pGunTransform{ pObject->GetTransform() };
	pGunTransform->SetShowImGui(true);
	
}

void DDM::ModelLoaderComponent::DropFileCallback(int count, const char** paths)
{
	if (!m_IsTextboxHovered)
		return;
	if (count > 0)
	{
		std::fill(std::begin(m_FilePath), std::end(m_FilePath), 0);
		

		for (int i{}; i < 125; i++)
		{
			if (paths[0][i] == '\0')
			{
				break;
			}
			else
			{
				m_FilePath[i] = paths[0][i];
			}
		}
	}
}
