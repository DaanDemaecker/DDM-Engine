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

#include "Managers/ConfigManager.h"

#include "Engine/Window.h"

DDM::ForwardRenderer::ForwardRenderer()
{
	auto surface{ VulkanObject::GetInstance().GetSurface() };

	// Get pointer to gpu object
	GPUObject* pGPUObject{ VulkanObject::GetInstance().GetGPUObject() };

	// Initialize the swapchain
	m_pSwapchainWrapper = std::make_unique<SwapchainWrapper>(pGPUObject, surface, VulkanObject::GetInstance().GetImageManager(), VulkanObject::GetInstance().GetMsaaSamples());

	// Initialize the renderpass
	CreateRenderpass(m_pSwapchainWrapper->GetFormat());

	// Create a single time command buffer
	auto commandBuffer{ VulkanObject::GetInstance().BeginSingleTimeCommands() };
	// Initialize swapchain
	m_pSwapchainWrapper->SetupImageViews(pGPUObject, VulkanObject::GetInstance().GetImageManager(), commandBuffer, m_pRenderpass.get());
	// End the single time command buffer
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);

	// Initialize the sync objects
	m_pSyncObjectManager = std::make_unique<SyncObjectManager>(pGPUObject->GetDevice(),VulkanObject::GetInstance().GetMaxFrames() );

	InitImgui();
}

DDM::ForwardRenderer::~ForwardRenderer()
{
}

void DDM::ForwardRenderer::Render()
{
	auto& vulkanObject{ DDM::VulkanObject::GetInstance() };

	auto currentFrame{ vulkanObject.GetCurrentFrame() };

	auto device{ vulkanObject.GetDevice() };
	auto& queueObject{ vulkanObject.GetQueueObject() };

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

void DDM::ForwardRenderer::RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	auto extent{ m_pSwapchainWrapper->GetExtent() };

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

	m_pRenderpass->BeginRenderPass(commandBuffer, m_pSwapchainWrapper->GetFrameBuffer(imageIndex, m_pRenderpass.get()), extent);

	SceneManager::GetInstance().RenderSkybox();

	SceneManager::GetInstance().Render();

	SceneManager::GetInstance().RenderTransparancy();

	// Render the ImGui
	m_pImGuiWrapper->Render(commandBuffer);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void DDM::ForwardRenderer::RecreateSwapChain()
{
	// Get a reference to the window struct
	auto& windowStruct{ DDM::Window::GetInstance().GetWindowStruct() };

	//Get the width and the height of the window
	glfwGetFramebufferSize(windowStruct.pWindow, &windowStruct.Width, &windowStruct.Height);

	// While width and height are 0, wait before continuing
	while (windowStruct.Width == 0 || windowStruct.Height == 0)
	{
		glfwGetFramebufferSize(windowStruct.pWindow, &windowStruct.Width, &windowStruct.Height);
		glfwWaitEvents();
	}

	// Wait until the device is idle
	vkDeviceWaitIdle(DDM::VulkanObject::GetInstance().GetDevice());

	// Create a single time command
	auto commandBuffer{ VulkanObject::GetInstance().BeginSingleTimeCommands() };


	std::vector<RenderpassWrapper*> renderpasses{ m_pRenderpass.get()};

	// Recreate the swapchain
	m_pSwapchainWrapper->RecreateSwapChain(DDM::VulkanObject::GetInstance().GetGPUObject(), DDM::VulkanObject::GetInstance().GetSurface(),
		VulkanObject::GetInstance().GetImageManager(),
		commandBuffer, renderpasses);

	// End single time command
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);
}

void DDM::ForwardRenderer::CreateRenderpass(VkFormat swapchainFormat)
{
	int swapchainImageAmount = m_pSwapchainWrapper->GetSwapchainImageAmount();

	// Create renderpass
	m_pRenderpass = std::make_unique<RenderpassWrapper>();

	// Create subpass
	std::unique_ptr<Subpass> subpass{std::make_unique<Subpass>()};

	// Get a reference to vulkanobject
	auto& vulkanObject = VulkanObject::GetInstance();

	VkSampleCountFlagBits msaaSamples = vulkanObject.GetMsaaSamples();

	m_pRenderpass->SetSampleCount(msaaSamples);

	std::unique_ptr<Attachment> colorAttachment = std::make_unique<Attachment>(swapchainImageAmount);

	colorAttachment->SetFormat(swapchainFormat);
	colorAttachment->SetClearColorValue({ 0.388f, 0.588f, 0.929f, 1.0f });

	// Create attachment description
	VkAttachmentDescription colorAttachmentDesc{};
	// et format to color format 
	colorAttachmentDesc.format = swapchainFormat;
	// Give max amount of samplples
	colorAttachmentDesc.samples = msaaSamples;
	// Set loadOp function to load op clear
	colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// Set storeOp functoin to store op store
	colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Set initial layout to undefined
	colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Set final layout to color attachment optimal
	colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;;


	colorAttachment->SetAttachmentDesc(colorAttachmentDesc);

	m_pRenderpass->AddAttachment(std::move(colorAttachment));


	auto depthAttachment = std::make_unique<Attachment>(swapchainImageAmount);

	auto depthFormat = VulkanUtils::FindDepthFormat(vulkanObject.GetPhysicalDevice());

	depthAttachment->SetAttachmentType(Attachment::kAttachmentType_DepthStencil);

	depthAttachment->SetFormat(depthFormat);

	VkAttachmentDescription depthAttachmentDesc{};
	depthAttachmentDesc.flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
	// Set format to depth format
	depthAttachmentDesc.format = depthFormat;
	// Give max amount of samples
	depthAttachmentDesc.samples = vulkanObject.GetMsaaSamples();
	// Set loadOp function to load op clear
	depthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// Set storeOp function to store op don't care
	depthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// Set stencilLoadOp function to load op don't care
	depthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	// Set store op function to store op don't care
	depthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// Set initial layout to undefined
	depthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Set final layout to depth stencil attachment optimal
	depthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


	depthAttachment->SetAttachmentDesc(depthAttachmentDesc);

	m_pRenderpass->AddAttachment(std::move(depthAttachment));



	auto colorResolveAttachment = std::make_unique<Attachment>(swapchainImageAmount);

	colorResolveAttachment->SetIsSwapchainImage(true);

	colorResolveAttachment->SetFormat(swapchainFormat);

	colorResolveAttachment->SetAttachmentType(Attachment::kAttachmentType_ColorResolve);

	// Create attachment description
	VkAttachmentDescription colorResolveDesc{};
	// Set format to swapchain format
	colorResolveDesc.format = swapchainFormat;
	// Set samples to 1
	colorResolveDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	// Set loadOp function to load op don't care
	colorResolveDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	// Set storeOp function to store op store
	colorResolveDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Set sentilLoadOp to load op don't care
	colorResolveDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	// Set stencilStoreOp to store op don't care
	colorResolveDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// Set initial layout to undefined
	colorResolveDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Set final layout to present src khr
	colorResolveDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	colorResolveAttachment->SetAttachmentDesc(colorResolveDesc);



	m_pRenderpass->AddAttachment(std::move(colorResolveAttachment));
	
	// Create attachment reference
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = kAttachment_COLOR;
	// Set layout to color attachment optimal
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = kAttachemnt_DEPTH;
	// Set layout to depth stencil attachment optimal
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorResolveRef{};
	colorResolveRef.attachment = kAttachment_COLORRESOLVE;
	// Set layout to color attachment optimal
	colorResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	subpass->AddReference(colorAttachmentRef);
	subpass->AddDepthRef(depthAttachmentRef);
	subpass->AddResolveRef(colorResolveRef);

	m_pRenderpass->AddSubpass(std::move(subpass));


	m_pRenderpass->CreateRenderPass();

	m_pSwapchainWrapper->AddFrameBuffers(m_pRenderpass.get());
}

void DDM::ForwardRenderer::InitImgui()
{
	// Create ImGui vulkan init info
	ImGui_ImplVulkan_InitInfo init_info = {};
	// Give the vulkan instance
	init_info.Instance = DDM::VulkanObject::GetInstance().GetVulkanInstance();
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
	m_pImGuiWrapper = std::make_unique<DDM::ImGuiWrapper>(init_info,
		VulkanObject::GetInstance().GetDevice(), static_cast<uint32_t>(VulkanObject::GetInstance().GetMaxFrames()), m_pRenderpass->GetRenderpass(), commandBuffer);
	// End the single time command buffer
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);
}

void DDM::ForwardRenderer::CleanupImgui()
{
	m_pImGuiWrapper = nullptr;
}

VkExtent2D DDM::ForwardRenderer::GetExtent()
{
	return m_pSwapchainWrapper->GetExtent();
}

DDM::RenderpassWrapper* DDM::ForwardRenderer::GetDefaultRenderpass()
{
	return m_pRenderpass.get();
}

void DDM::ForwardRenderer::AddDefaultPipelines()
{
	// Get config manager
	auto& configManager{ ConfigManager::GetInstance() };

	// Initialize default pipeline name 
	auto defaultPipelineName = configManager.GetString("DefaultPipelineName");

	// Add default pipeline
	VulkanObject::GetInstance().AddGraphicsPipeline(defaultPipelineName, {
		configManager.GetString("DefaultVertName"),
		configManager.GetString("DefaultFragName") });

	// Initialize default pipeline name 
	auto skyboxPipelineName = configManager.GetString("SkyboxPipelineName");

	// Add default pipeline
	VulkanObject::GetInstance().AddGraphicsPipeline(skyboxPipelineName, {
		configManager.GetString("SkyboxVertName"),
		configManager.GetString("SkyboxFragName") });
}
