// Attachment.cpp

// Header include
#include "Attachment.h"

#include "Vulkan/VulkanObject.h"

#include "Vulkan/VulkanManagers/ImageManager.h"

DDM3::Attachment::Attachment()
{
	m_Texture = std::make_shared<Texture>();
}

DDM3::Attachment::~Attachment()
{
	Cleanup();
}

void DDM3::Attachment::SetupImage(VkExtent2D extent, VkImageView swapchainImage)
{
	if (!m_ResetOnSetup)
		return;

	switch (m_Type)
	{
	case kAttachmentType_Color:
		SetupColorTexture(extent);
		break;
	case kAttachmentType_ColorResolve:
		SetupColorResolveTexture(extent, swapchainImage);
		break;
	case kAttachmentType_DepthStencil:
		SetupDepthImage(extent);
		break;
	default:
		break;
	}
}

void DDM3::Attachment::SetupColorTexture(VkExtent2D extent)
{
	Cleanup();

	auto& vulkanObject{ VulkanObject::GetInstance() };

	auto pImageManager = vulkanObject.GetImageManager();
	// Create the image for the color
	// Set tiling to optimal
	// Set usage to transient attachment and color attachment
	// Set properties to device local
	pImageManager->CreateImage(vulkanObject.GetGPUObject(), extent.width, extent.height, 1, m_AttachmentDesc.samples, m_Format,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *m_Texture);

	// Create image view for the color image
	m_Texture->imageView = pImageManager->CreateImageView(vulkanObject.GetDevice(), m_Texture->image, m_Format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void DDM3::Attachment::SetupColorResolveTexture(VkExtent2D extent, VkImageView swapchainImage)
{
	//m_Texture->imageView = swapchainImage;

	Cleanup();

	auto& vulkanObject{ VulkanObject::GetInstance() };

	auto pImageManager = vulkanObject.GetImageManager();
	// Create the image for the color
	// Set tiling to optimal
	// Set usage to transient attachment and color attachment
	// Set properties to device local
	pImageManager->CreateImage(vulkanObject.GetGPUObject(), extent.width, extent.height, 1, m_AttachmentDesc.samples, m_Format,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *m_Texture);

	// Create image view for the color image
	m_Texture->imageView = pImageManager->CreateImageView(vulkanObject.GetDevice(), m_Texture->image, m_Format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void DDM3::Attachment::SetupDepthImage(VkExtent2D extent)
{
	Cleanup();

	auto& vulkanObject{ VulkanObject::GetInstance() };

	auto pImageManager = vulkanObject.GetImageManager();
	

	// Create the image for the depth
	// Set tiling to optimal
	// Set usage to depth stencil attachment bit
	// Set properties to device local
	pImageManager->CreateImage(vulkanObject.GetGPUObject(), extent.width, extent.height, 1, m_AttachmentDesc.samples, m_Format,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		*m_Texture);

	// Create image view for the depth image
	m_Texture->imageView = pImageManager->CreateImageView(vulkanObject.GetDevice(), m_Texture->image, m_Format, VK_IMAGE_ASPECT_DEPTH_BIT, 1);


	auto commandBuffer{ vulkanObject.BeginSingleTimeCommands() };

	pImageManager->TransitionImageLayout(m_Texture->image, commandBuffer, m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);

	vulkanObject.EndSingleTimeCommands(commandBuffer);
}

void DDM3::Attachment::Cleanup()
{
	if (m_Texture)
	{
		m_Texture->Cleanup(VulkanObject::GetInstance().GetDevice());
	}
}
