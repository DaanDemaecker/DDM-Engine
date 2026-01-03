// MaterialSwitcher.h
// This component will be used to swap the material of MeshRenderComponent
// from a list of given materials

#ifndef _DDM_MATERIAL_SWITCHER_
#define _DDM_MATERIAL_SWITCHER_

// Parent include
#include "BaseClasses/Component.h"

// Standard library includes
#include <memory>
#include <map>
#include <string>

namespace DDM
{
	// Class forward declarations
	class MeshRenderComponent;
	class Material;

	class MaterialSwitcher final : public Component
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		MaterialSwitcher();

		/// <summary>
		/// Destructor
		/// </summary>
		~MaterialSwitcher();

		/// <summary>
		/// Register a new material with a key
		/// </summary>
		/// <param name="key: ">key associated with the material</param>
		/// <param name="pMaterial: ">pointer to the material</param>
		void RegisterMaterial(std::string& key, std::shared_ptr<Material> pMaterial);

		/// <summary>
		/// Register a new material with a key
		/// </summary>
		/// <param name="key: ">key associated with the material</param>
		/// <param name="pMaterial: ">pointer to the material</param>
		void RegisterMaterial(std::string&& key, std::shared_ptr<Material> pMaterial);

		/// <summary>
		/// Set current material by key
		/// </summary>
		/// <param name="key: ">key associated with the material</param>
		void SetMaterial(std::string key);

	private:
		// Pointer to the mesh renderer component
		std::shared_ptr<MeshRenderComponent> m_pMeshRenderer{ nullptr };

		// Map of registered materials
		std::map<std::string, std::shared_ptr<Material>> m_pRegisteredMaterials{};
	};
}

#endif // !_DDM_MATERIAL_SWITCHER_