// ImGuiWrapper.h
// This class wil function as a wrapper that handles the ImGui library

#ifndef ImGuiWrapperIncluded
#define ImGuiWrapperIncluded

// File includes
#include "Includes/VulkanIncludes.h"
#include "Includes/ImGuiIncludes.h"

namespace DDM
{
	class ImGuiWrapper final
	{
	public:
		// Constructor
		// Parameters:
		//     init_info: the initialization info for imgui
		//     device: the handle for the VkDevice
		//     maxFrames: the max frames in flight
		ImGuiWrapper(ImGui_ImplVulkan_InitInfo init_info, VkDevice device, uint32_t maxFrames,
			VkRenderPass renderPass, VkCommandBuffer commandBuffer);

		// Default destructor
		~ImGuiWrapper();

		// Delete default constructor
		ImGuiWrapper() = delete;

		// Delete copy and move functions
		ImGuiWrapper(ImGuiWrapper& other) = delete;
		ImGuiWrapper(ImGuiWrapper&& other) = delete;
		ImGuiWrapper& operator=(ImGuiWrapper& other) = delete;
		ImGuiWrapper& operator=(ImGuiWrapper&& other) = delete;

		// Cleanup
		// Parameters:
		//     device: handle for the VkDevice
		void Cleanup(VkDevice device);

		// Render the imgui
		// Parameters:
		//     commandBuffer: the commandBuffer used for rendering
		void Render(VkCommandBuffer commandBuffer);

	private:
		// Descriptorpool needed for ImGUI
		VkDescriptorPool m_DescriptorPool{};

		// Show the ImGui Demo
		bool m_ShowDemo{ false };

		// Initialization of descriptorpool
		// Parameters:
		//     device: handle for the VkDevice
		//     maxFrames: the amount of frames in flight
		void InitDescriptorPool(VkDevice device, uint32_t maxFrames);

	};
}
#endif // !ImGuiWrapperIncluded