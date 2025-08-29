// InfoComponent.cpp

// Header include
#include "InfoComponent.h"

//File includes
#include "Managers/TimeManager.h"
#include "Includes/DXGIIncludes.h"
#include "Vulkan/VulkanObject.h"
#include "Vulkan/VulkanWrappers/GPUObject.h"

// Standard library includes
#include <bitset>
#include "Psapi.h"
#include <windows.h>
#include <iostream>

DDM::InfoComponent::InfoComponent()
	:Component()
{
	// Code taken from:
	// https://asawicki.info/news_1695_there_is_a_way_to_query_gpu_memory_usage_in_vulkan_-_use_dxgi.html

	// Create DXGI factory
	IDXGIFactory4* dxgiFactory = nullptr;
	CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));

	// Fetch LUID of vulkan physical device
	auto vulkanLUID = VulkanObject::GetInstance().GetGPUObject()->GetDeviceLuid();

	// Loop through adapters until one is found that matches the vulkan physical device
	IDXGIAdapter1* tmpDxgiAdapter = nullptr;
	UINT adapterIndex = 0;
	while (dxgiFactory->EnumAdapters1(adapterIndex, &tmpDxgiAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		// Get description of current adapter
		DXGI_ADAPTER_DESC1 desc;
		tmpDxgiAdapter->GetDesc1(&desc);

		// Check if LUID matches the LUID of the vulkan device
		if (memcmp(&desc.AdapterLuid, vulkanLUID, VK_LUID_SIZE) == 0)
		{
			// If it matches, save current adapter
			tmpDxgiAdapter->QueryInterface(IID_PPV_ARGS(&m_DxgiAdapter));
		}

		// Release current adapter and increment index
		tmpDxgiAdapter->Release();
		++adapterIndex;
	}

	// Release factory
	dxgiFactory->Release();
}

DDM::InfoComponent::~InfoComponent()
{
	// Release adapter
	m_DxgiAdapter->Release();
}

void DDM::InfoComponent::Update()
{
	// Increase framecount and deltatime stat
	m_Frames++;
	m_DeltaTimeMS += DDM::TimeManager::GetInstance().GetDeltaTimeMS();

	// If amount of frames since last update superceeds amount of frames per update, query stats
	if (m_Frames >= m_FramesPerUpdate)
	{
		QueryStats();
	}
}

void DDM::InfoComponent::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	// Start tree
	if (ImGui::TreeNodeEx("Info", flags))
	{
		// Text for delta time, vram and memory
		ImGui::Text(m_DeltaTimeLabel.c_str());
		ImGui::Text(m_VRamLabel.c_str());
		ImGui::Text(m_MemoryLabel.c_str());

		ImGui::TreePop();
	}
}

void DDM::InfoComponent::QueryStats()
{
	// Update delta time label and reset values
	m_DeltaTimeLabel = std::string("Delta time: " + std::to_string(m_DeltaTimeMS / m_Frames) + " ms");

	m_Frames = 0;
	m_DeltaTimeMS = 0;

	// Update VRAM label
	m_VRamLabel = std::string("VRAM usage: " + std::to_string(GetVRAMUsage()) + " MB");

	// Update memory labem
	m_MemoryLabel = std::string("Memory usage: " + std::to_string(GetMemoryUsage()) + " MB");
}

int DDM::InfoComponent::GetVRAMUsage()
{
	// Query video memory info
	DXGI_QUERY_VIDEO_MEMORY_INFO info = {};
	m_DxgiAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info);

	// Convert usage from bits to megabytes and return
	return static_cast<int>(info.CurrentUsage / 1024 / 1024);
}

int DDM::InfoComponent::GetMemoryUsage()
{
	// Query memory info
	PROCESS_MEMORY_COUNTERS_EX pmc{};
	if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
	{
		// Convert usage from bits to megabytes and return
		SIZE_T memUsedBytes = pmc.PrivateUsage;
		double memUsedMB = memUsedBytes / (1024.0 * 1024.0);
		return static_cast<int>(memUsedMB);
	}

	return 0;
}