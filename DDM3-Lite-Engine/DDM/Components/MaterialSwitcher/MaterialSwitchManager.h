// MaterialSwitchManager.h
// This component will be used to swap the material of all registered MaterialSwitchers

#ifndef _MATERIAL_SWITCH_MANAGER_
#define _MATERIAL_SWITCH_MANAGER_


// Parent include
#include "BaseClasses/Component.h"

// Standard library includes
#include <memory>
#include <vector>
#include <string>

namespace DDM
{
	class MaterialSwitcher;

	class MaterialSwitchManager final : public Component
	{
	public:
		MaterialSwitchManager();
		
		~MaterialSwitchManager();

		void RegisterKey(std::string& key);

		void RegisterKey(std::string&& key);

		void SetCurrentKey(std::string& key);

		void SetCurrentKey(std::string&& key);

		void RegisterMaterialSwitcher(std::shared_ptr<MaterialSwitcher> pMaterialSwitcher);

		virtual void OnGUI() override;

	private:
		const std::string m_Label{"Set material"};

		int m_CurrentKey{};
		std::vector<std::string> m_AvailableKeys{};

		std::vector<std::shared_ptr<MaterialSwitcher>> m_pMaterialSwitchers{};

		void SwitchMaterial(int key);
	};
}

#endif // !_MATERIAL_SWITCH_MANAGER_