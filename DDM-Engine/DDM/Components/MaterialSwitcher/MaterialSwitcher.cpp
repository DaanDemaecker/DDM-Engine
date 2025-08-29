// MaterialSwitcher.cpp

// Header include
#include "MaterialSwitcher.h"

// File includes
#include "Components/MeshRenderer.h"
#include "DataTypes/Materials/Material.h"


DDM::MaterialSwitcher::MaterialSwitcher()
{
}

DDM::MaterialSwitcher::~MaterialSwitcher()
{
}

void DDM::MaterialSwitcher::RegisterMaterial(std::string& key, std::shared_ptr<Material> pMaterial)
{
	// Add material to map
	m_pRegisteredMaterials[key] = pMaterial;
}

void DDM::MaterialSwitcher::RegisterMaterial(std::string&& key, std::shared_ptr<Material> pMaterial)
{
	// Propagate to lvalue version
	RegisterMaterial(key, pMaterial);
}

void DDM::MaterialSwitcher::SetMaterial(std::string key)
{
	// Check if material with given key exists
	if (m_pRegisteredMaterials.contains(key))
	{
		// If mesh renderer is not yet set, try to get it
		if (m_pMeshRenderer == nullptr)
		{
			m_pMeshRenderer = GetComponent<DDM::MeshRenderComponent>();
			// If still null, print error and return
			if (m_pMeshRenderer == nullptr)
			{
				std::cerr << "MaterialSwitcher: No MeshRenderComponent found on GameObject " << GetOwner()->GetName() << std::endl;
				return;
			}
		}
		// Set the material on the mesh renderer
		m_pMeshRenderer->SetMaterial(m_pRegisteredMaterials[key]);
	}
	else
	{
		// Print error if no material with given key exists
		std::cerr << "MaterialSwitcher: No material found with key " << key << " on GameObject " << GetOwner()->GetName() << std::endl;
	}
}
