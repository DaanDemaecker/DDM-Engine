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
		std::string m_MemoryLabel{ "" };

		IDXGIAdapter3* m_DxgiAdapter{ nullptr };
	
		const int m_FramesPerUpdate{ 5 };
		int m_Frames{};
		float m_DeltaTimeMS{};

		void QueryStats();

		int GetVRAMUsage();

		int GetMemoryUsage();

		// Measurement variables and methods
		const int m_SampleSize{200};

		bool m_IsMeasuring{ false };

		int m_CurrentMeasurement{};
		int m_CurrentMeasurementFrame{};

		std::vector<std::vector<float>> m_DeltaTimeMeasurements{};
		std::vector<std::vector<float>> m_VRAMMeasurements{};

		void StartMeasurement();

		void EndMeasurement();

		void AddMeasurement();

		void RenderDeltaTimePlot();

		void RenderPlot(const std::vector<std::vector<float>>& samples);

		ImColor GetColorFromIndex(int index);

	};
}

#endif // !_INFO_COMPONENT_