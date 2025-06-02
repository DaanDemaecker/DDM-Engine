// DefaultRenderer.cpp

// Header include
#include "ForwardRenderer.h"

// File includes
#include "Managers/SceneManager.h"

#include "Vulkan/VulkanUtils.h"
#include "Vulkan/VulkanObject.h"
#include "Vulkan/VulkanManagers/SyncObjectManager.h"
#include "Vulkan/VulkanWrappers/SwapchainWrapper.h"
#include "Vulkan/VulkanWrappers/RenderpassWrapper.h"
#include "Vulkan/VulkanWrappers/ImGuiWrapper.h"
#include "Vulkan/VulkanManagers/ImageManager.h"
#include "Vulkan/VulkanManagers/CommandpoolManager.h"
#include "Vulkan/VulkanManagers/PipelineManager.h"
#include "Vulkan/VulkanManagers/BufferCreator.h"
#include "Vulkan/VulkanWrappers/GPUObject.h"

#include "Engine/Window.h"

DDM3::ForwardRenderer::ForwardRenderer()
{
	auto surface{ VulkanObject::GetInstance().GetSurface() };

	// Get pointer to gpu object
	GPUObject* pGPUObject{ VulkanObject::GetInstance().GetGPUObject() };

	// Initialize the swapchain
	m_pSwapchainWrapper = std::make_unique<SwapchainWrapper>(pGPUObject, surface, VulkanObject::GetInstance().GetImageManager(), VulkanObject::GetInstance().GetMsaaSamples(), m_AttachmentCount);

	// Initialize the renderpass
	CreateRenderpass(m_pSwapchainWrapper->GetFormat());

	// Create a single time command buffer
	auto commandBuffer{ VulkanObject::GetInstance().BeginSingleTimeCommands() };
	// Initialize swapchain
	m_pSwapchainWrapper->SetupImageViews(pGPUObject, VulkanObject::GetInstance().GetImageManager(), commandBuffer, GetRenderpass());
	// End the single time command buffer
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);

	// Initialize the sync objects
	m_pSyncObjectManager = std::make_unique<SyncObjectManager>(pGPUObject->GetDevice(),VulkanObject::GetInstance().GetMaxFrames() );

	InitImgui();
}

DDM3::ForwardRenderer::~ForwardRenderer()
{
}

void DDM3::ForwardRenderer::Render()
{
	auto& vulkanObject{ DDM3::VulkanObject::GetInstance() };

	auto currentFrame{ vulkanObject.GetCurrentFrame() };

	auto device{ vulkanObject.GetDevice() };
	auto queueObject{ vulkanObject.GetQueueObject() };

	vkWaitForFences(device, 1, &m_pSyncObjectManager->GetInFlightFence(currentFrame), VK_TRUE, UINT64_MAX);

	uint32_t imageIndex{};
	VkResult result = vkAcquireNextImageKHR(device, m_pSwapchainWrapper->GetSwapchain(),
		UINT64_MAX, m_pSyncObjectManager->GetImageAvailableSemaphore(currentFrame), VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	vkResetFences(device, 1, &m_pSyncObjectManager->GetInFlightFence(currentFrame));

	auto commandBuffer{ vulkanObject.GetCurrentCommandBuffer()};
	vkResetCommandBuffer(commandBuffer, 0);
	RecordCommandBuffer(commandBuffer, imageIndex);


	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_pSyncObjectManager->GetImageAvailableSemaphore(currentFrame) };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkSemaphore signalSemaphores[] = { m_pSyncObjectManager->GetRenderFinishedSemaphore(currentFrame) };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(queueObject.graphicsQueue, 1, &submitInfo, m_pSyncObjectManager->GetInFlightFence(currentFrame)) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_pSwapchainWrapper->GetSwapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(queueObject.presentQueue, &presentInfo);


	auto& window{ Window::GetInstance().GetWindowStruct() };

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.FrameBufferResized)
	{
		RecreateSwapChain();
		window.FrameBufferResized = false;
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}
}

void DDM3::ForwardRenderer::RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_pRenderpassWrapper->GetRenderpass();
	renderPassInfo.framebuffer = m_pSwapchainWrapper->GetFrameBuffer(imageIndex);


	auto extent{ m_pSwapchainWrapper->GetExtent() };

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = extent;
	
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.388f, 0.588f, 0.929f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(extent.width);
	viewport.height = static_cast<float>(extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	SceneManager::GetInstance().RenderSkybox();

	SceneManager::GetInstance().Render();

	// Render the ImGui
	m_pImGuiWrapper->Render(commandBuffer);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void DDM3::ForwardRenderer::RecreateSwapChain()
{
	// Get a reference to the window struct
	auto& windowStruct{ DDM3::Window::GetInstance().GetWindowStruct() };

	//Get the width and the height of the window
	glfwGetFramebufferSize(windowStruct.pWindow, &windowStruct.Width, &windowStruct.Height);

	// While width and height are 0, wait before continuing
	while (windowStruct.Width == 0 || windowStruct.Height == 0)
	{
		glfwGetFramebufferSize(windowStruct.pWindow, &windowStruct.Width, &windowStruct.Height);
		glfwWaitEvents();
	}

	// Wait until the device is idle
	vkDeviceWaitIdle(DDM3::VulkanObject::GetInstance().GetDevice());

	// Create a single time command
	auto commandBuffer{ VulkanObject::GetInstance().BeginSingleTimeCommands() };

	// Recreate the swapchain
	m_pSwapchainWrapper->RecreateSwapChain(DDM3::VulkanObject::GetInstance().GetGPUObject(), DDM3::VulkanObject::GetInstance().GetSurface(),
		VulkanObject::GetInstance().GetImageManager(),
		commandBuffer, m_pRenderpassWrapper->GetRenderpass());

	// End single time command
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);
}

void DDM3::ForwardRenderer::CreateRenderpass(VkFormat swapchainFormat)
{
	m_pRenderpassWrapper = std::make_unique<RenderpassWrapper>();

	VkSampleCountFlagBits msaaSamples = VulkanObject::GetInstance().GetMsaaSamples();

	// Create attachment description
	VkAttachmentDescription colorAttachment{};
	// et format to color format 
	colorAttachment.format = swapchainFormat;
	// Give max amount of samplples
	colorAttachment.samples = msaaSamples;
	// Set loadOp function to load op clear
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// Set storeOp functoin to store op store
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Set initial layout to undefined
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Set final layout to color attachment optimal
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;;

	// Create attachment reference
	VkAttachmentReference colorAttachmentRef{};
	// Set layout to color attachment optimal
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	m_pRenderpassWrapper->AddAttachment(colorAttachment, colorAttachmentRef);

	m_pRenderpassWrapper->AddDepthAttachment();

	m_pRenderpassWrapper->AddColorResolve(swapchainFormat);

	m_pRenderpassWrapper->CreateRenderPass();
}

void DDM3::ForwardRenderer::InitImgui()
{
	// Create ImGui vulkan init info
	ImGui_ImplVulkan_InitInfo init_info = {};
	// Give the vulkan instance
	init_info.Instance = DDM3::VulkanObject::GetInstance().GetVulkanInstance();
	// Give the physical device
	init_info.PhysicalDevice = VulkanObject::GetInstance().GetPhysicalDevice();
	// Give the logical device
	init_info.Device = VulkanObject::GetInstance().GetDevice();
	// Give the index of the graphics queue family
	init_info.QueueFamily = VulkanObject::GetInstance().GetQueueObject().graphicsQueueIndex;
	// Give the graphics queue
	init_info.Queue = VulkanObject::GetInstance().GetQueueObject().graphicsQueue;
	// Set pipeline cache to null handle
	init_info.PipelineCache = VK_NULL_HANDLE;
	// Set Allocator to null handle
	init_info.Allocator = VK_NULL_HANDLE;
	// Set min image count to the minimum image count of the swapchain
	init_info.MinImageCount = m_pSwapchainWrapper->GetMinImageCount();
	// Set the image count to the max amount of frames in flight
	init_info.ImageCount = static_cast<uint32_t>(VulkanObject::GetInstance().GetMaxFrames());
	// Give functoin for error handling
	init_info.CheckVkResultFn = [](VkResult /*err*/) { /* error handling */ };
	// Give the max amount of samples per mixel
	init_info.MSAASamples = VulkanObject::GetInstance().GetMsaaSamples();

	// Create a single time command buffer
	auto commandBuffer{ VulkanObject::GetInstance().BeginSingleTimeCommands() };
	// Initialize ImGui
	m_pImGuiWrapper = std::make_unique<DDM3::ImGuiWrapper>(init_info,
		VulkanObject::GetInstance().GetDevice(), static_cast<uint32_t>(VulkanObject::GetInstance().GetMaxFrames()), m_pRenderpassWrapper->GetRenderpass(), commandBuffer);
	// End the single time command buffer
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);
}

void DDM3::ForwardRenderer::CleanupImgui()
{
	m_pImGuiWrapper = nullptr;
}

VkExtent2D DDM3::ForwardRenderer::GetExtent()
{
	return m_pSwapchainWrapper->GetExtent();
}

VkRenderPass DDM3::ForwardRenderer::GetRenderpass()
{
	return m_pRenderpassWrapper->GetRenderpass();
}
