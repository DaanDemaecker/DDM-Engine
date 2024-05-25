#include "GameObject.h"
#include "../Components/TransformComponent.h"
#include "../Includes/ImGuiIncludes.h"

D3D::GameObject::~GameObject()
{
}

D3D::GameObject* D3D::GameObject::CreateNewObject(const std::string& name, const std::string& tag)
{
	auto pNewObject{ std::make_unique<GameObject>(name, tag) };

	pNewObject->Init();

	pNewObject->m_pParent = this;

	return m_pChildrenToAdd.emplace_back(std::move(pNewObject)).get();
}

void D3D::GameObject::RemoveAllChildren()
{
	m_pChildren.clear();
	m_pChildrenToAdd.clear();
}

void D3D::GameObject::SetParent(GameObject* pParent, bool worldPositionStays)
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

void D3D::GameObject::Init()
{
	m_pTransform = AddComponent<TransformComponent>();
}

void D3D::GameObject::OnSceneLoad()
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

void D3D::GameObject::OnSceneUnload()
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

void D3D::GameObject::StartFrame()
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

void D3D::GameObject::EarlyUpdate()
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

void D3D::GameObject::Update()
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

void D3D::GameObject::FixedUpdate()
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

void D3D::GameObject::LateUpdate()
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

void D3D::GameObject::Render() const
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

void D3D::GameObject::OnGUI()
{
	if (m_ShowImGui)
	{
		ImGui::Begin(m_Name.c_str(), &m_ShowGuiWindow);

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

void D3D::GameObject::Cleanup()
{
	for (auto& pChild : m_pChildren)
	{
		if (pChild->m_IsActive)
		{
			pChild->Cleanup();
		}
	}
}

void D3D::GameObject::Destroy()
{
	m_ShouldDestroy = true;
}
