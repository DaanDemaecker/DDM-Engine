// MaterialSwitchManager.cpp

// Header include
#include "MaterialSwitchManager.h"

// File includes
#include "Components/MaterialSwitcher/MaterialSwitcher.h"
#include "Includes/ImGuiIncludes.h"


DDM::MaterialSwitchManager::MaterialSwitchManager()
{
	// Enable ImGui for this component
	SetShowImGui(true);
}

DDM::MaterialSwitchManager::~MaterialSwitchManager()
{

}

void DDM::MaterialSwitchManager::RegisterKey(std::string& key)
{
	// Check if key is already registered, return if it is
	if (std::find(m_AvailableKeys.begin(), m_AvailableKeys.end(), key) != m_AvailableKeys.end())
	{
		return;
	}
	
	// Register key
	m_AvailableKeys.push_back(key);
}

void DDM::MaterialSwitchManager::RegisterKey(std::string&& key)
{
	// Propagate to lvalue version
	RegisterKey(key);
}

void DDM::MaterialSwitchManager::SetCurrentKey(std::string& key)
{
	// Find key in available keys
	for (int i = 0; i < m_AvailableKeys.size(); ++i)
	{
		if (m_AvailableKeys[i].compare(key) == 0)
		{
			m_CurrentKey = i;
			break;
		}
	}

	// Switch material
	SwitchMaterial(m_CurrentKey);
}

void DDM::MaterialSwitchManager::SetCurrentKey(std::string&& key)
{
	// Propagate to lvalue version
	SetCurrentKey(key);
}

void DDM::MaterialSwitchManager::RegisterMaterialSwitcher(std::shared_ptr<MaterialSwitcher> pMaterialSwitcher)
{
	// Check if material switcher is already registered, return if it is
	if (std::find(m_pMaterialSwitchers.begin(), m_pMaterialSwitchers.end(), pMaterialSwitcher) != m_pMaterialSwitchers.end())
	{
		return;
	}

	// Register material switcher
	m_pMaterialSwitchers.push_back(pMaterialSwitcher);
}


void DDM::MaterialSwitchManager::OnGUI()
{
	// Create a list of c-strings from the available keys
	auto cStringList = std::vector<const char*>();

	cStringList.reserve(m_AvailableKeys.size());

	for (auto& key : m_AvailableKeys)
	{
		cStringList.emplace_back(key.c_str());
	}

	// Store previous key to check if it changed
	int prevKey = m_CurrentKey;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	// Create a tree node for the material switcher
	if (ImGui::TreeNodeEx("Material switcher", flags))
	{
		ImGui::ListBox(m_Label.c_str(), &m_CurrentKey, cStringList.data(), static_cast<int>(cStringList.size()));
		ImGui::TreePop();
	}

	// If key changed, switch material
	if (prevKey != m_CurrentKey)
	{
		SwitchMaterial(m_CurrentKey);
	}
}

void DDM::MaterialSwitchManager::SwitchMaterial(int key)
{
	// Set material of all registered material switchers
	for (auto& switcher : m_pMaterialSwitchers)
	{
		switcher->SetMaterial(m_AvailableKeys[m_CurrentKey]);
	}
}
