// Attachment.cpp

// Header include
#include "Attachment.h"

// File includes
#include "Vulkan/VulkanObject.h"
#include "Vulkan/VulkanManagers/ImageManager/ImageManager.h"

DDM::Attachment::Attachment(int swapchainImageAmount)
{
	for (int i{}; i < swapchainImageAmount; ++i)
	{
		m_Textures.push_back(std::make_shared<Texture>());
	}
}

DDM::Attachment::~Attachment()
{
	Cleanup();
}

void DDM::Attachment::SetTexture(int index, VkImage image, VkImageView imageView)
{
	m_Textures[index]->image = image;
	m_Textures[index]->imageView = imageView;

	m_ResetOnSetup = false;
}

void DDM::Attachment::SetupImage(int index, VkExtent2D extent, VkImageView swapchainImage)
{
	if (!m_ResetOnSetup)
		return;

	CleanupImage(index);

	switch (m_Type)
	{
	case kAttachmentType_Color:
	case kAttachmentType_ColorResolve:
		SetupColorTexture(index, extent);
		break;
	case kAttachmentType_DepthStencil:
		SetupDepthImage(index, extent);
		break;
	default:
		break;
	}
}

VkClearColorValue DDM::Attachment::GetClearColorValue() const
{
	return m_ClearColorValue;
}

void DDM::Attachment::SetupColorTexture(int index, VkExtent2D extent)
{
	auto& vulkanObject{ VulkanObject::GetInstance() };

	auto pImageManager = vulkanObject.GetImageManager();
	// Create the image for the color
	// Set tiling to optimal
	// Set usage to transient attachment and color attachment
	// Set properties to device local
	pImageManager->CreateImage(vulkanObject.GetGPUObject(), extent.width, extent.height, 1, m_AttachmentDesc.samples, m_Format,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | (m_IsInput ? VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT : 0),
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *m_Textures[index]);

	// Create image view for the color image
	m_Textures[index]->imageView = pImageManager->CreateImageView(vulkanObject.GetDevice(), m_Textures[index]->image, m_Format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void DDM::Attachment::SetupDepthImage(int index, VkExtent2D extent)
{
	auto& vulkanObject{ VulkanObject::GetInstance() };

	auto pImageManager = vulkanObject.GetImageManager();
	

	// Create the image for the depth
	// Set tiling to optimal
	// Set usage to depth stencil attachment bit
	// Set properties to device local
	pImageManager->CreateImage(vulkanObject.GetGPUObject(), extent.width, extent.height, 1, m_AttachmentDesc.samples, m_Format,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | (m_IsInput ? VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT : 0),
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		*m_Textures[index]);

	// Create image view for the depth image
	m_Textures[index]->imageView = pImageManager->CreateImageView(vulkanObject.GetDevice(), m_Textures[index]->image, m_Format, VK_IMAGE_ASPECT_DEPTH_BIT, 1);


	auto commandBuffer{ vulkanObject.BeginSingleTimeCommands() };

	pImageManager->TransitionImageLayout(m_Textures[index]->image, commandBuffer, m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);

	vulkanObject.EndSingleTimeCommands(commandBuffer);
}

void DDM::Attachment::CleanupImage(int index)
{
	if (m_Textures[index])
	{
		m_Textures[index]->Cleanup(VulkanObject::GetInstance().GetDevice());
	}
}

void DDM::Attachment::Cleanup()
{
	if (m_ResetOnSetup)
	{
		for (int i{}; i < m_Textures.size(); ++i)
		{
			CleanupImage(i);
		}
	}
}
