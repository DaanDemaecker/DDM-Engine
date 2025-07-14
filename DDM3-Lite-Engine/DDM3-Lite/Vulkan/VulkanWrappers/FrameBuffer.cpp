// FrameBuffer.cpp

// Header include
#include "FrameBuffer.h"

//File includes
#include "Vulkan/VulkanObject.h"

#include "Vulkan/VulkanWrappers/Attachment.h"
#include "Vulkan/VulkanWrappers/RenderpassWrapper.h"

DDM3::FrameBuffer::FrameBuffer()
{

}

DDM3::FrameBuffer::~FrameBuffer()
{
	
	vkDestroyFramebuffer(VulkanObject::GetInstance().GetDevice(), m_FrameBuffer, nullptr);
}

void DDM3::FrameBuffer::CreateFrameBuffer(int index, RenderpassWrapper* renderpass, VkExtent2D extent, VkImageView swapchainImage)
{
	if (m_FrameBuffer != VK_NULL_HANDLE)
	{
		// If the framebuffer already exists, destroy it
		vkDestroyFramebuffer(VulkanObject::GetInstance().GetDevice(), m_FrameBuffer, nullptr);
		m_FrameBuffer = VK_NULL_HANDLE;
	}

	auto& attachmentList{ renderpass->GetAttachmentList() };

	std::vector<VkImageView> attachments{};

	for (auto& attachment : attachmentList)
	{
			attachments.push_back(attachment->GetTexture(index)->imageView);
	}


	// Create framebuffer create info
	VkFramebufferCreateInfo framebufferInfo{};
	// Set type to framebuffer create info
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	// Give handle of renderpass
	framebufferInfo.renderPass = renderpass->GetRenderpass();
	// Set attachment count to size of attachments
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	// Give pointer to data of attachments
	framebufferInfo.pAttachments = attachments.data();
	// Set width to swapchainextent width
	framebufferInfo.width = extent.width;
	// Set height to swapchain extent height
	framebufferInfo.height = extent.height;
	// Set amount of layers to 1
	framebufferInfo.layers = 1;

	// Create framebuffers
	if (vkCreateFramebuffer(VulkanObject::GetInstance().GetDevice(), &framebufferInfo, nullptr, &m_FrameBuffer) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create framebuffer!");
	}
}
