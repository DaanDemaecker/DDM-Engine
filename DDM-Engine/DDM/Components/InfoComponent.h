// InfoComponent.h
// This component will display information like the current frame rate and other useful information

#ifndef _INFO_COMPONENT_
#define _INFO_COMPONENT_


// Parent include
#include "BaseClasses/Component.h"
#include "Includes/ImGuiIncludes.h"

class IDXGIAdapter3;

namespace DDM
{
	class InfoComponent final : public Component
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		InfoComponent();

		/// <summary>
		/// Destructor
		/// </summary>
		~InfoComponent();

		/// <summary>
		/// Update function
		/// </summary>
		virtual void Update() override;

		/// <summary>
		/// OnGui function
		/// </summary>
		virtual void OnGUI() override;

	private:
		// Label for the delta time text in ImGui
		std::string m_DeltaTimeLabel{ "" };

		// Label for the vram text in ImGui
		std::string m_VRamLabel{ "" };

		// Label for the memory text in ImGui
		std::string m_MemoryLabel{ "" };

		// DirectX12 adapter
		IDXGIAdapter3* m_DxgiAdapter{ nullptr };
	
		// Amount of frames before updating labels
		const int m_FramesPerUpdate{ 5 };

		// Amount of frames since last update
		int m_Frames{};

		// Amount of time since last update in ms
		float m_DeltaTimeMS{};

		/// <summary>
		/// Query all stats
		/// </summary>
		void QueryStats();

		/// <summary>
		/// Get current VRAM usage
		/// </summary>
		/// <returns>Current VRAM usage</returns>
		int GetVRAMUsage();

		/// <summary>
		/// Get private memory usage
		/// </summary>
		/// <returns>Private memory usage</returns>
		int GetMemoryUsage();
	};
}

#endif // !_INFO_COMPONENT_