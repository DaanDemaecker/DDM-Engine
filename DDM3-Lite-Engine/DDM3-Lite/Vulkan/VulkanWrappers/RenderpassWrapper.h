// RenerpassWrapper.h
// This class will serve as a wrapper for the VkRenderPass object

#ifndef RenderpassWrapperIncluded
#define RenderpassWrapperIncluded

// File includdes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <vector>

namespace DDM3
{
	class RenderpassWrapper final
	{
	public:
		// Constructor
		// Parameters:
		//     device: handle of the VkDevice
		//     swapchainImageFormat: the format that the swapchain color image is in
		//     depthFormat: the format that the swapchain depth image is in
		//     msaaSamples: the max amount of samples per pixels
		RenderpassWrapper();

		// Default destructor
		~RenderpassWrapper();

		// Delete copy and move functions
		RenderpassWrapper(RenderpassWrapper& other) = delete;
		RenderpassWrapper(RenderpassWrapper&& other) = delete;
		RenderpassWrapper& operator=(RenderpassWrapper& other) = delete;
		RenderpassWrapper& operator=(RenderpassWrapper&& other) = delete;

		// Clean up allocated objects
		// Parameters:
		//     device: handle of the VkDevice
		void Cleanup(VkDevice device);

		// Get the handle of the renderpass
		VkRenderPass GetRenderpass() const { return m_RenderPass; }

		void AddAttachment(VkAttachmentDescription description, VkAttachmentReference reference);

		void AddAttachmentDescription(VkAttachmentDescription description);

		void AddDepthAttachment();

		void AddColorResolve(VkFormat swapchainFormat);

		void CreateRenderPass();
	private:
		std::vector<VkAttachmentDescription> m_Attachments{};

		VkAttachmentReference m_DepthAttachmentRef{};

		VkAttachmentReference m_ColorResolveAttachmentRef{};

		std::vector<VkAttachmentReference> m_ColorAttachmentRefs{};

		bool m_DepthAttachmentSet{false};

		bool m_ColorResolveSet{ false };

		//Renderpass
		VkRenderPass m_RenderPass{};

		//RenderpassInfo
		VkRenderPassBeginInfo m_RenderpassInfo{};

		// Initialize the renderpass
		// Parameters:
		//     device: handle of the VkDevice
		//     swapchainImageFormat: the format that the swapchain color image is in
		//     depthFormat: the format that the swapchain depth image is in
		//     msaaSamples: the max amount of samples per pixels
		void CreateRenderPass(VkDevice device, VkFormat swapchainImageFormat, VkFormat depthFormat,
			VkSampleCountFlagBits msaaSamples, int attachmentCount);

	};
}

#endif // !RenderpassWrapperIncluded
