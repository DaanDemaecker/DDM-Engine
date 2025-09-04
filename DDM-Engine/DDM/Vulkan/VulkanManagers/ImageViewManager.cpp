// ImageViewManager.cpp

// Header include
#include "ImageViewManager.h"

// File includes
#include "Vulkan/VulkanManagers/ImageManager/ImageManager.h"
#include "Vulkan/VulkanUtils.h"
#include "Vulkan/VulkanWrappers/GPUObject.h"
#include "Vulkan/VulkanObject.h"

#include "DataTypes/Image.h"


DDM::ImageViewManager::ImageViewManager(VkSampleCountFlagBits msaaSamples)
// Initialize max amount of samples per pixel
	:m_MsaaSamples{ msaaSamples }
{
}

DDM::ImageViewManager::~ImageViewManager()
{

}

void DDM::ImageViewManager::CreateColorResources(GPUObject* pGPUObject, VkFormat format, VkExtent2D swapchainExtent, DDM::ImageManager* pImageManager)
{
	// Create the image for the color
	// Set tiling to optimal
	// Set usage to transient attachment and color attachment
	// Set properties to device local
	pImageManager->CreateImage(swapchainExtent.width, swapchainExtent.height, 1, m_MsaaSamples, format,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pColorImage);

	// Create image view for the color image
	m_pColorImage->SetImageView(pImageManager->CreateImageView(pGPUObject->GetDevice(), m_pColorImage->GetImage(), format, VK_IMAGE_ASPECT_COLOR_BIT, 1));
}

void DDM::ImageViewManager::CreateDepthResources(GPUObject* pGPUObject, VkExtent2D swapchainExtent, DDM::ImageManager* pImageManager, VkCommandBuffer commandBuffer)
{
	// Get the depth formatµ
	auto depthFormat = VulkanUtils::FindDepthFormat(pGPUObject->GetPhysicalDevice());

	// Create the image for the depth
	// Set tiling to optimal
	// Set usage to depth stencil attachment bit
	// Set properties to device local
	pImageManager->CreateImage(swapchainExtent.width, swapchainExtent.height, 1, m_MsaaSamples, depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_pDepthImage);

	// Create image view for the depth image
	m_pDepthImage->SetImageView(
		pImageManager->CreateImageView(pGPUObject->GetDevice(), m_pDepthImage->GetImage(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1));

	// Transition the depth image from undefined to depth stencil attachment optimal
	pImageManager->TransitionImageLayout(m_pDepthImage->GetImage(), commandBuffer, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}

VkImageView DDM::ImageViewManager::GetColorImageView() const
{
	return m_pColorImage->GetImageView();
}

VkImageView DDM::ImageViewManager::GetDepthImageView() const
{
	return m_pDepthImage->GetImageView();
}

const std::vector<std::shared_ptr<DDM::Image>>& DDM::ImageViewManager::GetColorImages() const
{
	return m_pColorImages;
}
