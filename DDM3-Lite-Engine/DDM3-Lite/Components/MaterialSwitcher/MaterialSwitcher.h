// MaterialSwitcher.h
// This component will be used to swap the material of MeshRenderComponent
// from a list of given materials

#ifndef _MATERIAL_SWITCHER_
#define _MATERIAL_SWITCHER_

// Parent include
#include "BaseClasses/Component.h"

// Standard library includes
#include <memory>
#include <map>
#include <string>

namespace DDM3
{
	class MeshRenderComponent;
	class Material;

	class MaterialSwitcher final : public Component
	{
	public:
		MaterialSwitcher();

		~MaterialSwitcher();

		void RegisterMaterial(std::string& key, std::shared_ptr<Material> pMaterial);

		void RegisterMaterial(std::string&& key, std::shared_ptr<Material> pMaterial);

		void SetMaterial(std::string key);

	private:
		std::shared_ptr<MeshRenderComponent> m_pMeshRenderer{ nullptr };

		std::map<std::string, std::shared_ptr<Material>> m_pRegisteredMaterials{};
	};
}

#endif // !_MATERIAL_SWITCHER_