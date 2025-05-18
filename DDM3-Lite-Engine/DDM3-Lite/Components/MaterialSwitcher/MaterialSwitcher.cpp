// MaterialSwitcher.cpp

// Header include
#include "MaterialSwitcher.h"

// File includes
#include "Components/MeshRenderComponent.h"
#include "DataTypes/Materials/Material.h"


DDM3::MaterialSwitcher::MaterialSwitcher()
{
}

DDM3::MaterialSwitcher::~MaterialSwitcher()
{
}

void DDM3::MaterialSwitcher::RegisterMaterial(std::string& key, std::shared_ptr<Material> pMaterial)
{
	m_pRegisteredMaterials[key] = pMaterial;
}

void DDM3::MaterialSwitcher::RegisterMaterial(std::string&& key, std::shared_ptr<Material> pMaterial)
{
	RegisterMaterial(key, pMaterial);
}

void DDM3::MaterialSwitcher::SetMaterial(std::string key)
{
	if (m_pRegisteredMaterials.contains(key))
	{
		if (m_pMeshRenderer == nullptr)
		{
			m_pMeshRenderer = GetComponent<DDM3::MeshRenderComponent>();
		}
		m_pMeshRenderer->SetMaterial(m_pRegisteredMaterials[key]);
	}
}
