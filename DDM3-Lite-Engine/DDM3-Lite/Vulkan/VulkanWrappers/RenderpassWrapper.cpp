// RenderpassWrapper.cpp

// Header include
#include "RenderpassWrapper.h"
#include "Vulkan/VulkanObject.h"
#include "Vulkan/VulkanUtils.h"
#include "Vulkan/VulkanWrappers/Attachment.h"

// Standard library includes
#include <array>
#include <stdexcept>

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
	attachment->SetAttachmentRefIndex(m_AttachmentList.size());

	m_AttachmentList.push_back(std::move(attachment));
}

void DDM3::RenderpassWrapper::AddDepthAttachment(std::unique_ptr<Attachment> attachment)
{
	m_DepthAttachmentSet = true;

	attachment->SetAttachmentRefIndex(m_AttachmentList.size());

	m_DepthAttachment = std::move(attachment);
}

void DDM3::RenderpassWrapper::AddColorResolveAttachment(std::unique_ptr<Attachment> attachment)
{
	m_ColorResolveSet = true;

	attachment->SetAttachmentRefIndex(m_AttachmentList.size() + (m_DepthAttachmentSet ? 1 : 0));

	m_ColorResolveAttachment = std::move(attachment);
}

void DDM3::RenderpassWrapper::CreateRenderPass()
{
	std::vector<VkAttachmentReference>attachmentRefs{};
	attachmentRefs.reserve(m_AttachmentList.size());

	for (auto& attachment : m_AttachmentList)
	{
		attachmentRefs.emplace_back(attachment->GetAttachmentRef());
	}


	// Create subpass description
	VkSubpassDescription subpass{};
	// Set pipelinje bind point to graphics
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	// Set color attachment count to 1
	subpass.colorAttachmentCount = static_cast<uint32_t>(m_AttachmentList.size());
	// Give pointer to color attachment reference
	subpass.pColorAttachments = attachmentRefs.data();

	if (m_DepthAttachmentSet)
	{
		auto attachment = 
		// Give pointer to depth attachment reference
		subpass.pDepthStencilAttachment = &m_DepthAttachment->GetAttachmentRef();
	}

	if (m_ColorResolveSet)
	{
		// Give pointer to color attachment resole reference
		subpass.pResolveAttachments = &m_ColorResolveAttachment->GetAttachmentRef();
	}

	// Create subpass dependency
	VkSubpassDependency dependency{};
	// Set source subpass to external
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	// Set destination subpass to 0
	dependency.dstSubpass = 0;
	// Set source stagemask to color attachment output and early fragment tests
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	// Set source access mask to 0
	dependency.srcAccessMask = 0;
	// Set destination stage mask to color atachment output and early fragment tests
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	// Set destination access mask to color attachment write and depth stencil attachment write
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


	std::vector<VkAttachmentDescription> attachments{};
	attachments.reserve(m_AttachmentList.size() +
		(m_DepthAttachmentSet ? 1 : 0) +
		(m_ColorResolveSet ? 1 : 0));

	for (auto& attachment : m_AttachmentList)
	{
		attachments.emplace_back(attachment->GetAttachmentDesc());
	}

	if (m_DepthAttachmentSet)
	{
		attachments.push_back(m_DepthAttachment->GetAttachmentDesc());
	}

	if (m_ColorResolveSet)
	{
		attachments.push_back(m_ColorResolveAttachment->GetAttachmentDesc());
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
	renderPassInfo.subpassCount = 1;
	// Give pointer to subpass count
	renderPassInfo.pSubpasses = &subpass;
	// Set dependency count to 1
	renderPassInfo.dependencyCount = 1;
	// Give pointer to dependency
	renderPassInfo.pDependencies = &dependency;

	// Create the renderpass
	if (vkCreateRenderPass(VulkanObject::GetInstance().GetDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create render pass!");
	}
}

void DDM3::RenderpassWrapper::CreateRenderPass(VkDevice device, VkFormat swapchainImageFormat, VkFormat depthFormat, VkSampleCountFlagBits msaaSamples, int attachmentCount)
{
	std::vector<VkAttachmentDescription> colorAttachments(attachmentCount);
	std::vector<VkAttachmentReference> colorAttachmentRefs(attachmentCount);


	for (int i{}; i < attachmentCount; ++i)
	{
		// Create attachment description
		VkAttachmentDescription colorAttachment{};
		// et format to color format 
		colorAttachment.format = swapchainImageFormat;
		// Give max amount of samplples
		colorAttachment.samples = msaaSamples;
		// Set loadOp function to load op clear
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		// Set storeOp functoin to store op store
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		// Set initial layout to undefined
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		// Set final layout to color attachment optimal
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// Create attachment reference
		VkAttachmentReference colorAttachmentRef{};
		// Set attachment to 0
		colorAttachmentRef.attachment = 0;
		// Set layout to color attachment optimal
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		colorAttachments[i] = colorAttachment;
		colorAttachmentRefs[i] = colorAttachmentRef;
	}


	// Create attachment description
	VkAttachmentDescription depthAttachment{};
	// Set format to depth format
	depthAttachment.format = depthFormat;
	// Give max amount of samples
	depthAttachment.samples = msaaSamples;
	// Set loadOp function to load op clear
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// Set storeOp function to store op don't care
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// Set stencilLoadOp function to load op don't care
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	// Set store op function to store op don't care
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// Set initial layout to undefined
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Set final layout to depth stencil attachment optimal
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Create attachment reference
	VkAttachmentReference depthAttachmentRef{};
	// Set attachmen to 1
	depthAttachmentRef.attachment = attachmentCount;
	// Set layout to depth stencil attachment optimal
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	

	// Create attachment description
	VkAttachmentDescription colorAttachmentResolve{};
	// Set format to swapchain format
	colorAttachmentResolve.format = swapchainImageFormat;
	// Set samples to 1
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	// Set loadOp function to load op don't care
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	// Set storeOp function to store op store
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Set sentilLoadOp to load op don't care
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	// Set stencilStoreOp to store op don't care
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// Set initial layout to undefined
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Set final layout to present src khr
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Create attachment reference
	VkAttachmentReference colorAttachmentResolveRef{};
	// Set attachment to 2
	colorAttachmentResolveRef.attachment = attachmentCount + 1;
	// Set layout to color attachment optimal
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Create subpass description
	VkSubpassDescription subpass{};
	// Set pipelinje bind point to graphics
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	// Set color attachment count to 1
	subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
	// Give pointer to color attachment reference
	subpass.pColorAttachments = colorAttachmentRefs.data();
	// Give pointer to depth attachment reference
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	// Give pointer to color attachment resole reference
	subpass.pResolveAttachments = &colorAttachmentResolveRef;

	// Create subpass dependency
	VkSubpassDependency dependency{};
	// Set source subpass to external
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	// Set destination subpass to 0
	dependency.dstSubpass = 0;
	// Set source stagemask to color attachment output and early fragment tests
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	// Set source access mask to 0
	dependency.srcAccessMask = 0;
	// Set destination stage mask to color atachment output and early fragment tests
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	// Set destination access mask to color attachment write and depth stencil attachment write
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	// Create array the size of all attachments
	std::vector<VkAttachmentDescription> attachments(attachmentCount + 2);

	for (int i{}; i < colorAttachments.size(); ++i)
	{
		attachments[i] = colorAttachments[i];
	}
	attachments[attachmentCount] = depthAttachment;
	attachments[attachmentCount + 1] = colorAttachmentResolve;

	// Create rander pass create info
	VkRenderPassCreateInfo renderPassInfo{};
	// Set type to render pass create info
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	// Set attachment count to the size of attachments
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	// Set pAttachments as pointer to date of attachments
	renderPassInfo.pAttachments = attachments.data();
	// Set subpasscount to 1
	renderPassInfo.subpassCount = 1;
	// Give pointer to subpass count
	renderPassInfo.pSubpasses = &subpass;
	// Set dependency count to 1
	renderPassInfo.dependencyCount = 1;
	// Give pointer to dependency
	renderPassInfo.pDependencies = &dependency;

	// Create the renderpass
	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create render pass!");
	}
}