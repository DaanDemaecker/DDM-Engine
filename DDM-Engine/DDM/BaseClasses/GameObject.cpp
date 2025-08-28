// GameObject.cpp

// Header include
#include "GameObject.h"

// File includes
#include "Components/TransformComponent.h"
#include "Includes/ImGuiIncludes.h"

DDM::GameObject::~GameObject()
{
}

DDM::GameObject* DDM::GameObject::CreateNewObject(const std::string& name, const std::string& tag)
{
	// Create new game object
	auto pNewObject{ std::make_unique<GameObject>(name, tag) };

	// Initialize the new object
	pNewObject->Init();

	// Set parent to this object
	pNewObject->m_pParent = this;

	// Add new object to list of children to add next frame and return pointer to it
	return m_pChildrenToAdd.emplace_back(std::move(pNewObject)).get();
}

void DDM::GameObject::RemoveAllChildren()
{
	// Clear all children and children to add
	m_pChildren.clear();
	m_pChildrenToAdd.clear();
}

void DDM::GameObject::SetParent(GameObject* pParent, bool worldPositionStays)
{
	// If new parent or current parent is null, set local position to world position
	if (pParent == nullptr || m_pParent == nullptr)
	{
		GetTransform()->SetLocalPosition(GetTransform()->GetWorldPosition());
	}
	else
	{
		// If world position should stay, calculate new relative position
		if (worldPositionStays)
		{
			GetTransform()->SetLocalPosition(GetTransform()->GetWorldPosition() -
				GetParent()->GetTransform()->GetWorldPosition());
		}
		// Set dirty flag
		GetTransform()->SetPositionDirtyFlag();
	}


	std::unique_ptr<GameObject> child;

	if (m_pParent != nullptr)
	{
		// Find this object in the list of children of the current parent
		for (auto it = m_pParent->m_pChildren.begin(); it != m_pParent->m_pChildren.end(); ++it)
		{
			if (it->get() == this)
			{
				// Save the child and remove it from the list
				child = std::move(*it);
				m_pParent->m_pChildren.erase(it);
				break;
			}
		}
	}

	// Set new parent
	m_pParent = pParent;

	// If new parent is not null, add this object to the list of children of the new parent
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
	// Add a transform component by default
	m_pTransform = AddComponent<TransformComponent>();
}

void DDM::GameObject::OnSceneLoad()
{
	// Propagate scene load to components and children
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
	// Mark object for destruction
	m_ShouldDestroy = true;


	// Propagate scene unload to components and children
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
	// Add children that were created last frame
	for (size_t i{}; i < m_pChildrenToAdd.size(); ++i)
	{
		m_pChildren.push_back(std::move(m_pChildrenToAdd[i]));
	}

	// Clear the list of children to add
	m_pChildrenToAdd.clear();

	// Start frame for all children
	for (auto& pChild : m_pChildren)
	{
		pChild->StartFrame();
	}
}

void DDM::GameObject::EarlyUpdate()
{
	// Early update for all components
	for (auto& component : m_pComponents)
	{
		if (component->IsActive())
		{
			component->EarlyUpdate();
		}
	}

	// Early update for all active children
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
	// Update for all active components
	for (auto& component : m_pComponents)
	{
		if (component->IsActive())
		{
			component->Update();
		}
	}

	// Update for all active children
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
	// Fixed update for all active components
	for (auto& component : m_pComponents)
	{
		if (component->IsActive())
		{
			component->FixedUpdate();
		}
	}

	// Fixed update for all active children
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
	// Late update for all active components
	for (auto& component : m_pComponents)
	{
		if (component->IsActive())
		{
			component->LateUpdate();
		}
	}

	// Late update for all active children
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
	// Remove components and children that are marked for destruction
	m_pComponents.erase(std::remove_if(m_pComponents.begin(), m_pComponents.end(), [](std::shared_ptr<Component>& pComponent)
		{
			return pComponent->ShouldDestroy();
		}), m_pComponents.end());

	m_pChildren.erase(std::remove_if(m_pChildren.begin(), m_pChildren.end(), [](std::unique_ptr<GameObject>& pChild)
		{
			return pChild->ShouldDestroy();
		}), m_pChildren.end());


	// Post update for all components
	for (auto& pChild : m_pChildren)
	{
		pChild->PostUpdate();
	}
}

void DDM::GameObject::RenderDepth() const
{
	// Render depth for all active components
	for (auto& component : m_pComponents)
	{
		if (component->IsActive())
		{
			component->RenderDepth();
		}
	}

	// Render depth for all active children
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
	// Render for all active components
	for (auto& component : m_pComponents)
	{
		if (component->IsActive())
		{
			component->Render();
		}
	}

	// Render for all active children
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
	// Render transparancy for all active components
	for (auto& component : m_pComponents)
	{
		if (component->IsActive())
		{
			component->RenderTransparancy();
		}
	}

	// Render transparancy for all active children
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
	// If object should show ImGui, create a window with the name of the object
	if (m_ShowImGui)
	{
		// Create a window
		ImGui::Begin(m_Name.c_str(), &m_ShowGuiWindow);

		// Create a button
		if (ImGui::Button("Destroy object"))
		{
			// Call the function with the text input
			Destroy();
		}

		// Render ImGui for all components that want to show it
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

		// End the window
		ImGui::End();
	}

	// Render ImGui for all active children
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
	// Mark object for destruction
	m_ShouldDestroy = true;
}
