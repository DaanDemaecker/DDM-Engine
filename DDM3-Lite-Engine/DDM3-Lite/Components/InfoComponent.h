// InfoComponent.h
// This component will display information like the current frame rate and other useful information

#ifndef _INFO_COMPONENT_
#define _INFO_COMPONENT_


// Parent include
#include "BaseClasses/Component.h"

class IDXGIAdapter3;

namespace DDM3
{
	class InfoComponent final : public Component
	{
	public:
		/// <summary>
		/// Default constructor, initializes the DXGI adapter
		/// </summary>
		InfoComponent();

		/// <summary>
		/// Destructor, releases the DXGI adapter
		/// </summary>
		~InfoComponent();

		virtual void Update() override;

		virtual void OnGUI() override;

	private:
		std::string m_DeltaTimeLabel{ "" };
		std::string m_VRamLabel{ "" };

		IDXGIAdapter3* m_DxgiAdapter{ nullptr };
	
		const int m_FramesPerUpdate{ 5 };
		int m_Frames{};
		int m_DeltaTimeMS{};

		void QueryStats();

		int GetVRAMUsage();

		// Measurement variables and methods
		const int m_SampleSize{1000};

		bool m_IsMeasuring{ false };

		int m_CurrentMeasurement{};
		int m_CurrentMeasurementFrame{};

		std::vector<std::vector<float>> m_DeltaTimeMeasurements{};
		std::vector<std::vector<float>> m_VRAMMeasurements{};

		void StartMeasurement();

		void EndMeasurement();

		void AddMeasurement();

		void RenderDeltaTimePlot();
	};
}

#endif // !_INFO_COMPONENT_