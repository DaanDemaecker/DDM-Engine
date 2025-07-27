// RenderpassWrapper.cpp

// Header include
#include "RenderpassWrapper.h"
#include "Vulkan/VulkanObject.h"
#include "Vulkan/VulkanUtils.h"
#include "Vulkan/VulkanWrappers/Attachment.h"

// Standard library includes
#include <array>
#include <stdexcept>
#include <iostream>

DDM3::RenderpassWrapper::RenderpassWrapper()
{
}

DDM3::RenderpassWrapper::~RenderpassWrapper()
{
	Cleanup(DDM3::VulkanObject::GetInstance().GetDevice());
}

void DDM3::RenderpassWrapper::Cleanup(VkDevice device)
{
	// Destroy the renderpass
	vkDestroyRenderPass(device, m_RenderPass, nullptr);
}


void DDM3::RenderpassWrapper::AddAttachment(std::unique_ptr<Attachment> attachment)
{
	m_AttachmentList.push_back(std::move(attachment));
}

bool DDM3::RenderpassWrapper::IsColorResolveSet() const
{
	for (auto& attachment : m_AttachmentList)
	{
		if (attachment->GetAttachmentType() == Attachment::kAttachmentType_ColorResolve)
		{
			return true;
		}
	}

	return false;
}

void DDM3::RenderpassWrapper::BeginRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer frameBuffer, VkExtent2D extent, bool clearDepth)
{

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = GetRenderpass();
	renderPassInfo.framebuffer = frameBuffer;

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = extent;

	std::vector<VkClearValue> clearValues;
	clearValues.reserve(m_AttachmentList.size());

	for (const auto& attachment : m_AttachmentList)
	{
		VkClearValue clearValue{};
		switch (attachment->GetAttachmentType())
		{
		case Attachment::kAttachmentType_Color:
		case Attachment::kAttachmentType_ColorResolve:
			clearValue.color = attachment->GetClearColorValue();
			break;
		case Attachment::kAttachmentType_DepthStencil:
			clearValue.depthStencil = attachment->GetClearDepthStencilValue();
			break;
		default:
			break;
		}
		clearValues.push_back(clearValue);
	}

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void DDM3::RenderpassWrapper::AddSubpass(std::unique_ptr<Subpass> subpass)
{
	m_pSubpasses.push_back(std::move(subpass));
}

void DDM3::RenderpassWrapper::AddDependency(VkSubpassDependency dependency)
{
	m_Dependencies.push_back(dependency);
}

void DDM3::RenderpassWrapper::CreateRenderPass()
{
	std::vector<VkAttachmentDescription> attachments(m_AttachmentList.size());

	for (int i{}; i < m_AttachmentList.size(); ++i)
	{
		attachments[i] = m_AttachmentList[i]->GetAttachmentDesc();
	}

	std::vector<VkSubpassDescription> subpasses(m_pSubpasses.size());

	for (int i{}; i < m_pSubpasses.size(); ++i)
	{
		subpasses[i] = m_pSubpasses[i]->GetDescription();
	}

	// Create rander pass create info
	VkRenderPassCreateInfo renderPassInfo{};
	// Set type to render pass create info
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	// Set attachment count to the size of attachments
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	// Set pAttachments as pointer to date of attachments
	renderPassInfo.pAttachments = attachments.data();
	// Set subpasscount to 1
	renderPassInfo.subpassCount = subpasses.size();
	// Give pointer to subpass count
	renderPassInfo.pSubpasses = subpasses.data();
	// Set dependency count to 1
	renderPassInfo.dependencyCount = m_Dependencies.size();
	// Give pointer to dependency
	renderPassInfo.pDependencies = m_Dependencies.data();

	// Create the renderpass
	if (vkCreateRenderPass(VulkanObject::GetInstance().GetDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create render pass!");
	}
}