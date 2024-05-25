// ModelLoaderComponent.cpp

// File includes
#include "ModelLoaderComponent.h"
#include "../Includes/ImGuiIncludes.h"
#include "../Managers/SceneManager.h"
#include "../Engine/Scene.h"
#include "../DataTypes/Mesh.h"
#include "../Components/MeshRenderComponent.h"
#include "../Components/TransformComponent.h"

// Standard library includes
#include <iostream>

D3D::ModelLoaderComponent::ModelLoaderComponent()
	:Component()
{
}

void D3D::ModelLoaderComponent::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	if (ImGui::TreeNodeEx("Model Loader", flags))
	{
		ImGui::InputText("Object name", m_ObjectName, IM_ARRAYSIZE(m_ObjectName));
		ImGui::InputText("File path", m_FilePath, IM_ARRAYSIZE(m_FilePath));
		
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

	auto pMesh{ std::make_shared<D3D::Mesh>(m_FilePath) };

	auto pObjectModel{ pObject->AddComponent<D3D::MeshRenderComponent>() };
	pObjectModel->SetMesh(pMesh);

	auto pObjectTransform{ pObject->GetTransform() };
	pObjectTransform->SetShowImGui(true);
}
