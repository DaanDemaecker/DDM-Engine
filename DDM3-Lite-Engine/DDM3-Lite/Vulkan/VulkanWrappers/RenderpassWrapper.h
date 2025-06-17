// RenerpassWrapper.h
// This class will serve as a wrapper for the VkRenderPass object

#ifndef RenderpassWrapperIncluded
#define RenderpassWrapperIncluded

// File includdes
#include "Includes/VulkanIncludes.h"
#include "Vulkan/VulkanWrappers/Attachment.h"

// Standard library includes
#include <vector>
#include <memory>

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

		void AddAttachment(std::unique_ptr<Attachment> attachment);

		void AddDepthAttachment(std::unique_ptr<Attachment> attachment);

		void AddColorResolveAttachment(std::unique_ptr<Attachment> attachment);

		void CreateRenderPass();

		std::vector<std::unique_ptr<Attachment>>& GetAttachmentList() { return m_AttachmentList; }

		std::unique_ptr<Attachment>& GetDepthAttachment() { return m_DepthAttachment; }

		std::unique_ptr<Attachment>& GetColorResolveAttachment() { return m_ColorResolveAttachment; }

		bool IsColorResolveSet() const { return m_ColorResolveSet;}

		void BeginRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer frameBuffer, VkExtent2D extent);

		void SetSampleCount(VkSampleCountFlagBits sampleCount) { m_SampleCount = sampleCount; }
		VkSampleCountFlagBits GetSampleCount() const { return m_SampleCount; }

	private:
		std::vector<std::unique_ptr<Attachment>> m_AttachmentList{};

		std::unique_ptr<Attachment> m_DepthAttachment{};

		std::unique_ptr<Attachment> m_ColorResolveAttachment{};

		bool m_DepthAttachmentSet{false};

		bool m_ColorResolveSet{ false };

		//Renderpass
		VkRenderPass m_RenderPass{};

		//RenderpassInfo
		VkRenderPassBeginInfo m_RenderpassInfo{};

		VkSampleCountFlagBits m_SampleCount{ VK_SAMPLE_COUNT_1_BIT };

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
