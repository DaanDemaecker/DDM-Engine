// InfoComponent.cpp

// Header include
#include "InfoComponent.h"

//File includes
#include "Includes/ImGuiIncludes.h"
#include "Managers/TimeManager.h"
#include <Includes/DXGIIncludes.h>
#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/VulkanWrappers/GPUObject.h"

DDM3::InfoComponent::InfoComponent()
	:Component()
{
	// Code taken from:
	// https://asawicki.info/news_1695_there_is_a_way_to_query_gpu_memory_usage_in_vulkan_-_use_dxgi.html

	IDXGIFactory4* dxgiFactory = nullptr;
	CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));

	IDXGIAdapter1* tmpDxgiAdapter = nullptr;
	UINT adapterIndex = 0;
	while (dxgiFactory->EnumAdapters1(adapterIndex, &tmpDxgiAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 desc;
		tmpDxgiAdapter->GetDesc1(&desc);
		if (memcmp(&desc.AdapterLuid, VulkanRenderer::GetInstance().GetGPUObject()->GetDeviceLuid(), VK_LUID_SIZE) == 0)
		{
			tmpDxgiAdapter->QueryInterface(IID_PPV_ARGS(&m_DxgiAdapter));
		}
		tmpDxgiAdapter->Release();
		++adapterIndex;
	}

	dxgiFactory->Release();
}

DDM3::InfoComponent::~InfoComponent()
{
	m_DxgiAdapter->Release();
}

void DDM3::InfoComponent::Update()
{
	m_Frames++;
	m_DeltaTimeMS += DDM3::TimeManager::GetInstance().GetDeltaTimeMS();

	if (m_Frames >= m_FramesPerUpdate)
	{
		QueryStats();
	}

	if (m_IsMeasuring)
	{
		AddMeasurement();
	}
}

void DDM3::InfoComponent::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	if (ImGui::TreeNodeEx("Info window", flags))
	{
		ImGui::Text(m_DeltaTimeLabel.c_str());
		ImGui::Text(m_VRamLabel.c_str());

		if (ImGui::Button("Start measurement"))
		{
			if (!m_IsMeasuring)
			{
				StartMeasurement();
			}
		}

		if (m_DeltaTimeMeasurements.size() > 0)
		{
			RenderDeltaTimePlot();
		}

		ImGui::TreePop();
	}
}

void DDM3::InfoComponent::QueryStats()
{
	m_DeltaTimeLabel = std::string("Delta time: " + std::to_string(static_cast<int>(m_DeltaTimeMS / m_Frames)) + " ms");
	m_Frames = 0;
	m_DeltaTimeMS = 0;

	m_VRamLabel = std::string("VRAM usage: " + std::to_string(GetVRAMUsage()) + " MB");

}

int DDM3::InfoComponent::GetVRAMUsage()
{

	DXGI_QUERY_VIDEO_MEMORY_INFO info = {};
	m_DxgiAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info);

	return info.CurrentUsage / 1024 / 1024;
}

void DDM3::InfoComponent::StartMeasurement()
{
	std::cout << "Starting measurement" << std::endl;

	m_IsMeasuring = true;

	m_DeltaTimeMeasurements.push_back(std::vector<float>());
	m_DeltaTimeMeasurements[m_CurrentMeasurement].reserve(m_SampleSize);

	m_VRAMMeasurements.push_back(std::vector<float>());
	m_VRAMMeasurements[m_CurrentMeasurement].reserve(m_SampleSize);
}

void DDM3::InfoComponent::EndMeasurement()
{
	std::cout << "Ending measurement" << std::endl;

	m_IsMeasuring = false;
	m_CurrentMeasurement++;
	m_CurrentMeasurementFrame = 0;
}

void DDM3::InfoComponent::AddMeasurement()
{
	++m_CurrentMeasurementFrame;

	if (m_CurrentMeasurement < m_DeltaTimeMeasurements.size())
	{
		m_DeltaTimeMeasurements[m_CurrentMeasurement].emplace_back(DDM3::TimeManager::GetInstance().GetDeltaTimeMS());
	}

	if (m_CurrentMeasurement < m_DeltaTimeMeasurements.size())
	{
		m_VRAMMeasurements[m_CurrentMeasurement].emplace_back(GetVRAMUsage());
	}


	if (m_CurrentMeasurementFrame >= m_SampleSize)
	{
		EndMeasurement();
	}
}

void DDM3::InfoComponent::RenderDeltaTimePlot()
{
	ImGui::Text("Delta time plot");
	ImGui::PlotLines("Delta time", m_DeltaTimeMeasurements[0].data(), m_SampleSize, 0, nullptr, 0.0f, 100.0f, ImVec2(0, 10));
	ImGui::PlotLines("VRAM usage", m_VRAMMeasurements[0].data(), m_SampleSize, 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));
}
