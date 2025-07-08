// SwapchainWrapper.cpp

// Header include
#include "SwapchainWrapper.h"

// File includes
#include "Engine/Window.h"

#include "GPUObject.h"

#include "Vulkan/VulkanManagers/ImageViewManager.h"
#include "Vulkan/VulkanManagers/ImageManager.h"

#include "Vulkan/VulkanWrappers/FrameBuffer.h"
#include "Vulkan/VulkanWrappers/RenderpassWrapper.h"

#include "Vulkan/VulkanUtils.h"
#include "Vulkan/VulkanObject.h"

// Standard library includes
#include <stdexcept>
#include <algorithm>

DDM3::SwapchainWrapper::SwapchainWrapper(GPUObject* pGPUObject, VkSurfaceKHR surface,
	DDM3::ImageManager* pImageManager, VkSampleCountFlagBits msaaSamples)
{
	// Initialize the swapchain
	CreateSwapChain(pGPUObject, surface);
	// Initialize the image views
	CreateSwapchainImageViews(pGPUObject->GetDevice(), pImageManager);
}

DDM3::SwapchainWrapper::~SwapchainWrapper()
{
	for (auto& framebufferList : m_Framebuffers)
	{
		for (auto& framebuffer : framebufferList.second)
		{
			framebuffer.reset();
		}
	}

	Cleanup(VulkanObject::GetInstance().GetDevice());
}

void DDM3::SwapchainWrapper::SetupImageViews(GPUObject* pGPUObject, DDM3::ImageManager* pImageManager,
	VkCommandBuffer commandBuffer, RenderpassWrapper* renderPass)
{
	for (auto& attachment : renderPass->GetAttachmentList())
	{
		attachment->SetupImage(m_SwapChainExtent, VK_NULL_HANDLE);
	}

	// Initialize frame buffers
	CreateFramebuffers(pGPUObject->GetDevice(), renderPass);
}

void DDM3::SwapchainWrapper::AddFrameBuffers(RenderpassWrapper* renderpass)
{
	m_Framebuffers[renderpass] = std::vector<std::unique_ptr<FrameBuffer>>();

	// Resize framebuffers to size of imageviews
	m_Framebuffers[renderpass].resize(m_SwapChainImages.size());


	for (int i{}; i < m_Framebuffers[renderpass].size(); ++i)
	{
		m_Framebuffers[renderpass][i] = std::make_unique<FrameBuffer>();
	}
}

void DDM3::SwapchainWrapper::SetupSwapchain(GPUObject* pGPUObject, VkSurfaceKHR surface,
	DDM3::ImageManager* pImageManager, VkCommandBuffer commandBuffer, std::vector<RenderpassWrapper*>& renderpasses)
{
	// Initalize the swapchain
	CreateSwapChain(pGPUObject, surface);
	// Initialize swapchain image views
	CreateSwapchainImageViews(pGPUObject->GetDevice(), pImageManager);
	// Setup color and depth resources

	for (auto& renderpass : renderpasses)
	{
		SetupImageViews(pGPUObject, pImageManager, commandBuffer, renderpass);
	}
}

void DDM3::SwapchainWrapper::CreateSwapChain(GPUObject* pGPUObject, VkSurfaceKHR surface)
{
	// Get device
	auto device{ pGPUObject->GetDevice() };

	// Get swapchain support details
	DDM3::SwapChainSupportDetails swapChainSupport = VulkanUtils::QuerySwapChainSupport(pGPUObject->GetPhysicalDevice(), surface);
	// Get the format for the swapchain surface
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	// Get presentmode for the swapchain
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	// Get extent for swapchain
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	// Set minimum image count to minimum image count of swapchain capabilities
	m_MinImageCount = swapChainSupport.capabilities.minImageCount;
	// Create an image count and set it to the minimum image count of swapchain capabilities + 1
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	// If maximum image count is greater than 0 and image count is greater than maximum image count
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		// Set image count to maximum image count of swapchain capabilities
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	// Creaate swapchain create info
	VkSwapchainCreateInfoKHR createInfo{};
	// Set type to swapchain create info
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	// Give surface handle
	createInfo.surface = surface;
	// Set minimum image count
	createInfo.minImageCount = imageCount;
	// Set format to the surface format
	createInfo.imageFormat = surfaceFormat.format;
	// Set color space to surface color space
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	// Set extent to chosen swapchain extent
	createInfo.imageExtent = extent;
	// Set array layers to 1
	createInfo.imageArrayLayers = 1;
	// Set image usage to color attachment
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Get the queuefamily indices
	DDM3::QueueFamilyIndices indices = VulkanUtils::FindQueueFamilies(pGPUObject->GetPhysicalDevice(), surface);
	// Create array of graphics and present family indices
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	// If graphics and present family indices are not equal
	if (indices.graphicsFamily != indices.presentFamily)
	{
		//Image is owned by queue family and ownership must be transfered
		// Set sharing mode to concurrent
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		// Set index count to 2
		createInfo.queueFamilyIndexCount = 2;
		// Give array of indices
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		///Images can be used across multiple queue families without transfer of ownership
		// Set sharing mode to exclusove
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// Set index count to 0
		createInfo.queueFamilyIndexCount = 0;
		// Give nullptr for indices
		createInfo.pQueueFamilyIndices = nullptr;
	}

	// Set pre transform to current transform of swapchain capabilities
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	// Set composite alpha to opague
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	// Set presentmode to chosen presentmode
	createInfo.presentMode = presentMode;
	// Set clipped to 2
	createInfo.clipped = VK_TRUE;
	// Set old swapchain to null handle
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	// Create the swapchain
	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create swap chain!");
	}

	// Get the swapchain image count
	vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, nullptr);
	// Resize swapchain images to size of image count
	m_SwapChainImages.resize(imageCount);
	// Get swapchain images 
	vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, m_SwapChainImages.data());

	// Save swapchain format
	m_SwapChainImageFormat = surfaceFormat.format;
	// Save swapchain extent
	m_SwapChainExtent = extent;
}

void DDM3::SwapchainWrapper::CreateSwapchainImageViews(VkDevice device, DDM3::ImageManager* pImageManager)
{
	// Resize image views to the size of images
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	// Loop trough the amount of images
	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		// Create image view
		m_SwapChainImageViews[i] = pImageManager->CreateImageView(device, m_SwapChainImages[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

void DDM3::SwapchainWrapper::Cleanup(VkDevice device)
{
	// Loop trough the amount of image views
	for (size_t i = 0; i < m_SwapChainImageViews.size(); ++i)
	{
		if (m_SwapChainImageViews[i] != VK_NULL_HANDLE)
		{
			// Destroy the image view
			vkDestroyImageView(device, m_SwapChainImageViews[i], nullptr);
		}
	}

	// Destroy the swapchain
	vkDestroySwapchainKHR(device, m_SwapChain, nullptr);

}

void DDM3::SwapchainWrapper::RecreateSwapChain(GPUObject* pGPUObject, VkSurfaceKHR surface,
	DDM3::ImageManager* pImageManager, VkCommandBuffer commandBuffer, std::vector<RenderpassWrapper*>& renderpasses)
{
	// Call cleanup function to destroy all allocated objects
	Cleanup(pGPUObject->GetDevice());

	// Set swapchain again
	SetupSwapchain(pGPUObject, surface, pImageManager, commandBuffer, renderpasses);
}

VkFramebuffer DDM3::SwapchainWrapper::GetFrameBuffer(uint32_t index, RenderpassWrapper* renderpass)
{
	return m_Framebuffers[renderpass][index]->GetFrameBuffer();
}


VkExtent2D DDM3::SwapchainWrapper::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	// If the width of the current extent isn't equal to the maximum uint32_t, return the current extent
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	// If not create new values
	else
	{
		// Create width and height ints
		int width, height;
		// Get the width and height of the window
		glfwGetFramebufferSize(Window::GetInstance().GetWindowStruct().pWindow, &width, &height);
		// Create extent that is the size of the window
		VkExtent2D actualExtent =
		{
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		// Clamp width between minimum and maximum width capabilities
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		// Clamlp height between minimum and maximum height capabilities
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		// Return the newly created extent object
		return actualExtent;
	}
}

void DDM3::SwapchainWrapper::CreateFramebuffers(VkDevice device, RenderpassWrapper* renderpass)
{
	if (!m_Framebuffers.contains(renderpass))
		return;

	// Loop trough the amount of imageViews
	for (size_t i = 0; i < m_SwapChainImageViews.size(); ++i)
	{
		m_Framebuffers[renderpass][i]->CreateFrameBuffer(renderpass, m_SwapChainExtent, renderpass->IsColorResolveSet() ? m_SwapChainImageViews[i] : VK_NULL_HANDLE);
	}
}

VkSurfaceFormatKHR DDM3::SwapchainWrapper::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	// Loop trough the available formats
	for (const auto& availableFormat : availableFormats)
	{
		// I format is equal to B8G8R8A8 and colorspace is srgb nonlinear, return format
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	// If no suitable format was found, return first in the vector of formats
	return availableFormats[0];
}

VkPresentModeKHR DDM3::SwapchainWrapper::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	// Loop trough the available presentModes
	for (const auto& availablePresentMode : availablePresentModes)
	{
		// If current presentmode is mailbox, return it
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	// If no suitable format was found, return fifo mode
	return VK_PRESENT_MODE_FIFO_KHR;
}