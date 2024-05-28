// ModelLoaderComponent.cpp

// File includes
#include "ModelLoaderComponent.h"
#include "../Includes/ImGuiIncludes.h"
#include "../Managers/SceneManager.h"
#include "../Engine/Scene.h"
#include "../DataTypes/Mesh.h"
#include "../DataTypes/Materials/MultiMaterial.h"
#include "../Components/MeshRenderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Engine/Window.h"

// Standard library includes
#include <iostream>
#include <algorithm>

D3D::ModelLoaderComponent::ModelLoaderComponent()
	:Component()
{
	auto boundCallback = std::bind(&D3D::ModelLoaderComponent::DropFileCallback, this, std::placeholders::_1, std::placeholders::_2);


	Window::GetInstance().AddCallback(this, boundCallback);
}

void D3D::ModelLoaderComponent::OnGUI()
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

void D3D::ModelLoaderComponent::LoadObject()
{
	auto pObject{ SceneManager::GetInstance().GetActiveScene()->CreateGameObject(m_ObjectName)};
	pObject->SetShowImGui(true);

	std::shared_ptr<D3D::MultiMaterial> pMaterial{ std::make_shared<D3D::MultiMaterial>("MultiShader") };
	auto pMesh{ std::make_shared<D3D::Mesh>(m_FilePath) };

	auto pMeshRenderer{ pObject->AddComponent<D3D::MeshRenderComponent>() };
	pMeshRenderer->SetShowImGui(true);
	pMeshRenderer->SetMesh(pMesh);
	pMeshRenderer->SetMaterial(pMaterial);

	auto pGunTransform{ pObject->GetTransform() };
	pGunTransform->SetShowImGui(true);
	
}

void D3D::ModelLoaderComponent::DropFileCallback(int count, const char** paths)
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
