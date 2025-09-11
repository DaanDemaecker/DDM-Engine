// MaterialSwitchManager.h
// This component will be used to swap the material of all registered MaterialSwitchers

#ifndef _DDM_MATERIAL_SWITCH_MANAGER_
#define _DDM_MATERIAL_SWITCH_MANAGER_


// Parent include
#include "BaseClasses/Component.h"

// Standard library includes
#include <memory>
#include <vector>
#include <string>

namespace DDM
{
	// Class forward declarations
	class MaterialSwitcher;

	class MaterialSwitchManager final : public Component
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		MaterialSwitchManager();
		
		/// <summary>
		/// Destructor
		/// </summary>
		~MaterialSwitchManager();

		// Delete copy and move operations
		MaterialSwitchManager(const MaterialSwitchManager&) = delete;
		MaterialSwitchManager(MaterialSwitchManager&&) = delete;

		MaterialSwitchManager& operator=(const MaterialSwitchManager&) = delete;
		MaterialSwitchManager& operator=(MaterialSwitchManager&&) = delete;

		/// <summary>
		/// Register a new key
		/// </summary>
		/// <param name="key: ">name of the key</param>
		void RegisterKey(std::string& key);

		/// <summary>
		/// Register a new key
		/// </summary>
		/// <param name="key: ">name of the key</param>
		void RegisterKey(std::string&& key);

		/// <summary>
		/// Set current key
		/// </summary>
		/// <param name="key: ">name of the key</param>
		void SetCurrentKey(std::string& key);

		/// <summary>
		/// Set current key
		/// </summary>
		/// <param name="key: ">name of the key</param>
		void SetCurrentKey(std::string&& key);

		/// <summary>
		/// Register a new material switcher component
		/// </summary>
		/// <param name="pMaterialSwitcher: ">pointer to the material switcher component</param>
		void RegisterMaterialSwitcher(std::shared_ptr<MaterialSwitcher> pMaterialSwitcher);
		
		/// <summary>
		/// OnGui function
		/// </summary>
		virtual void OnGUI() override;

	private:
		// Label for the ImGui combo box
		const std::string m_Label{"Set material"};

		// Current key index
		int m_CurrentKey{};

		// List of available keys
		std::vector<std::string> m_AvailableKeys{};

		// List of registered material switchers
		std::vector<std::shared_ptr<MaterialSwitcher>> m_pMaterialSwitchers{};

		/// <summary>
		/// Switch the current material
		/// </summary>
		/// <param name="key: ">index of the key</param>
		void SwitchMaterial(int key);
	};
}

#endif // !_DDM_MATERIAL_SWITCH_MANAGER_