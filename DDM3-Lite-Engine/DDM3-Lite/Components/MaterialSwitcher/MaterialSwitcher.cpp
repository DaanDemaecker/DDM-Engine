// MaterialSwitcher.cpp

// Header include
#include "MaterialSwitcher.h"

// File includes
#include "Components/MeshRenderComponent.h"
#include "DataTypes/Materials/Material.h"


DDM::MaterialSwitcher::MaterialSwitcher()
{
}

DDM::MaterialSwitcher::~MaterialSwitcher()
{
}

void DDM::MaterialSwitcher::RegisterMaterial(std::string& key, std::shared_ptr<Material> pMaterial)
{
	m_pRegisteredMaterials[key] = pMaterial;
}

void DDM::MaterialSwitcher::RegisterMaterial(std::string&& key, std::shared_ptr<Material> pMaterial)
{
	RegisterMaterial(key, pMaterial);
}

void DDM::MaterialSwitcher::SetMaterial(std::string key)
{
	if (m_pRegisteredMaterials.contains(key))
	{
		if (m_pMeshRenderer == nullptr)
		{
			m_pMeshRenderer = GetComponent<DDM::MeshRenderComponent>();
		}
		m_pMeshRenderer->SetMaterial(m_pRegisteredMaterials[key]);
	}
}
