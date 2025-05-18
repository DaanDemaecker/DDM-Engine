// MaterialSwitchManager.cpp

// Header include
#include "MaterialSwitchManager.h"

// File includes
#include "Components/MaterialSwitcher/MaterialSwitcher.h"
#include "Includes/ImGuiIncludes.h"


DDM3::MaterialSwitchManager::MaterialSwitchManager()
{
	SetShowImGui(true);
}

DDM3::MaterialSwitchManager::~MaterialSwitchManager()
{

}

void DDM3::MaterialSwitchManager::RegisterKey(std::string& key)
{
	m_AvailableKeys.push_back(key);
}

void DDM3::MaterialSwitchManager::RegisterKey(std::string&& key)
{
	RegisterKey(key);
}

void DDM3::MaterialSwitchManager::SetCurrentKey(std::string& key)
{
	for (int i = 0; i < m_AvailableKeys.size(); ++i)
	{
		if (m_AvailableKeys[i].compare(key) == 0)
		{
			m_CurrentKey = i;
			break;
		}
	}

	SwitchMaterial(m_CurrentKey);
}

void DDM3::MaterialSwitchManager::SetCurrentKey(std::string&& key)
{
	SetCurrentKey(key);
}

void DDM3::MaterialSwitchManager::RegisterMaterialSwitcher(std::shared_ptr<MaterialSwitcher> pMaterialSwitcher)
{
	m_pMaterialSwitchers.push_back(pMaterialSwitcher);
}


void DDM3::MaterialSwitchManager::OnGUI()
{
	auto cStringList = std::vector<const char*>();

	cStringList.reserve(m_AvailableKeys.size());

	for (auto& key : m_AvailableKeys)
	{
		cStringList.emplace_back(key.c_str());
	}

	int prevKey = m_CurrentKey;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	if (ImGui::TreeNodeEx("Material switcher", flags))
	{
		ImGui::ListBox(m_Label.c_str(), &m_CurrentKey, cStringList.data(), cStringList.size());
		ImGui::TreePop();
	}


	if (prevKey != m_CurrentKey)
	{
		SwitchMaterial(m_CurrentKey);
	}
}

void DDM3::MaterialSwitchManager::SwitchMaterial(int key)
{
	for (auto& switcher : m_pMaterialSwitchers)
	{
		switcher->SetMaterial(m_AvailableKeys[m_CurrentKey]);
	}
}
