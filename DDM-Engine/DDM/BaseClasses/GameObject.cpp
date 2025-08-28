#include "GameObject.h"
#include "../Components/TransformComponent.h"
#include "../Includes/ImGuiIncludes.h"

DDM::GameObject::~GameObject()
{
}

DDM::GameObject* DDM::GameObject::CreateNewObject(const std::string& name, const std::string& tag)
{
	auto pNewObject{ std::make_unique<GameObject>(name, tag) };

	pNewObject->Init();

	pNewObject->m_pParent = this;

	return m_pChildrenToAdd.emplace_back(std::move(pNewObject)).get();
}

void DDM::GameObject::RemoveAllChildren()
{
	m_pChildren.clear();
	m_pChildrenToAdd.clear();
}

void DDM::GameObject::SetParent(GameObject* pParent, bool worldPositionStays)
{
	if (pParent == nullptr || m_pParent == nullptr)
	{
		GetTransform()->SetLocalPosition(GetTransform()->GetWorldPosition());
	}
	else
	{
		if (worldPositionStays)
		{
			GetTransform()->SetLocalPosition(GetTransform()->GetWorldPosition() -
				GetParent()->GetTransform()->GetWorldPosition());
		}
		GetTransform()->SetPositionDirtyFlag();
	}

	std::unique_ptr<GameObject> child;

	if (m_pParent != nullptr)
	{
		for (auto it = m_pParent->m_pChildren.begin(); it != m_pParent->m_pChildren.end(); it++)
		{
			if (it->get() == this)
			{
				child = std::move(*it);
				m_pParent->m_pChildren.erase(it);
				break;
			}
		}
	}

	m_pParent = pParent;

	if (m_pParent != nullptr)
	{
		if (child == nullptr)
		{
			child = std::unique_ptr<GameObject>(this);
		}

		m_pParent->m_pChildren.emplace_back(std::move(child));
	}
}

void DDM::GameObject::Init()
{
	m_pTransform = AddComponent<TransformComponent>();
}

void DDM::GameObject::OnSceneLoad()
{

	for (auto& component : m_pComponents)
	{
		component->OnSceneLoad();
	}

	for (auto& child : m_pChildren)
	{
		child->OnSceneLoad();
	}
}

void DDM::GameObject::OnSceneUnload()
{
	m_ShouldDestroy = true;

	for (auto& component : m_pComponents)
	{
		component->OnSceneUnload();
	}

	for (auto& child : m_pChildren)
	{
		child->OnSceneUnload();
	}
}

void DDM::GameObject::StartFrame()
{
	for (size_t i{}; i < m_pChildrenToAdd.size(); ++i)
	{
		m_pChildren.push_back(std::move(m_pChildrenToAdd[i]));
	}

	m_pChildrenToAdd.clear();

	for (auto& pChild : m_pChildren)
	{
		if (pChild->m_IsActive)
		{
			pChild->StartFrame();
		}
	}
}

void DDM::GameObject::EarlyUpdate()
{
	for (auto& component : m_pComponents)
	{
		component->EarlyUpdate();
	}

	for (auto& pChild : m_pChildren)
	{
		if (pChild->m_IsActive)
		{
			pChild->EarlyUpdate();
		}
	}
}

void DDM::GameObject::Update()
{
	for (auto& component : m_pComponents)
	{
		component->Update();
	}

	for (auto& pChild : m_pChildren)
	{
		if (pChild->m_IsActive)
		{
			pChild->Update();
		}
	}
}

void DDM::GameObject::FixedUpdate()
{
	for (auto& component : m_pComponents)
	{
		component->FixedUpdate();
	}

	for (auto& pChild : m_pChildren)
	{
		if (pChild->m_IsActive)
		{
			pChild->FixedUpdate();
		}
	}
}

void DDM::GameObject::LateUpdate()
{
	for (auto& component : m_pComponents)
	{
		component->LateUpdate();
	}

	for (auto& pChild : m_pChildren)
	{
		if (pChild->m_IsActive)
		{
			pChild->LateUpdate();
		}
	}
}

void DDM::GameObject::PostUpdate()
{
	m_pComponents.erase(std::remove_if(m_pComponents.begin(), m_pComponents.end(), [](std::shared_ptr<Component>& pComponent)
		{
			return pComponent->ShouldDestroy();
		}), m_pComponents.end());

	m_pChildren.erase(std::remove_if(m_pChildren.begin(), m_pChildren.end(), [](std::unique_ptr<GameObject>& pChild)
		{
			return pChild->ShouldDestroy();
		}), m_pChildren.end());


	for (auto& pChild : m_pChildren)
	{
		if (pChild->IsActive())
		{
			pChild->PostUpdate();
		}
	}
}

void DDM::GameObject::RenderDepth() const
{
	for (auto& component : m_pComponents)
	{
		component->RenderDepth();
	}

	for (auto& pChild : m_pChildren)
	{
		if (pChild->m_IsActive)
		{
			pChild->RenderDepth();
		}
	}
}

void DDM::GameObject::Render() const
{
	for (auto& component : m_pComponents)
	{
		component->Render();
	}

	for (auto& pChild : m_pChildren)
	{
		if (pChild->m_IsActive)
		{
			pChild->Render();
		}
	}
}

void DDM::GameObject::RenderTransparancy() const
{
	for (auto& component : m_pComponents)
	{
		component->RenderTransparancy();
	}

	for (auto& pChild : m_pChildren)
	{
		if (pChild->m_IsActive)
		{
			pChild->RenderTransparancy();
		}
	}
}

void DDM::GameObject::OnGUI()
{
	if (m_ShowImGui)
	{
		ImGui::Begin(m_Name.c_str(), &m_ShowGuiWindow);

		// Create a button
		if (ImGui::Button("Destroy object"))
		{
			// Call the function with the text input
			Destroy();
		}

		if (m_ShowGuiWindow)
		{
			for (auto& component : m_pComponents)
			{
				if (component->ShouldShowImGui())
				{
					component->OnGUI();
				}
			}
		}

		ImGui::End();
	}

	for (auto& pChild : m_pChildren)
	{
		if (pChild->m_IsActive)
		{
			pChild->OnGUI();
		}
	}
}

void DDM::GameObject::Destroy()
{
	m_ShouldDestroy = true;
}
