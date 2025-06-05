// DeferredRenderer.cpp

// Header include 
#include "DeferredRenderer.h"

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
#include "Managers/SceneManager.h"

#include "Managers/ConfigManager.h"

DDM3::DeferredRenderer::DeferredRenderer()
{
	auto surface{ VulkanObject::GetInstance().GetSurface() };

	// Get pointer to gpu object
	GPUObject* pGPUObject{ VulkanObject::GetInstance().GetGPUObject() };

	// Initialize the swapchain
	m_pSwapchainWrapper = std::make_unique<SwapchainWrapper>(pGPUObject, surface, VulkanObject::GetInstance().GetImageManager(), VulkanObject::GetInstance().GetMsaaSamples());

	CreateRenderpass();

	auto commandBuffer = VulkanObject::GetInstance().BeginSingleTimeCommands();
	m_pSwapchainWrapper->SetupImageViews(pGPUObject, VulkanObject::GetInstance().GetImageManager(), commandBuffer, m_pRenderpassWrapper.get());
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);

	// Initialize the sync objects
	m_pSyncObjectManager = std::make_unique<SyncObjectManager>(pGPUObject->GetDevice(), VulkanObject::GetInstance().GetMaxFrames());

	//InitImgui();
}

DDM3::DeferredRenderer::~DeferredRenderer()
{
	
}

void DDM3::DeferredRenderer::Render()
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

	auto commandBuffer{ vulkanObject.GetCurrentCommandBuffer() };
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

VkExtent2D DDM3::DeferredRenderer::GetExtent()
{
	return m_pSwapchainWrapper->GetExtent();
}

VkRenderPass DDM3::DeferredRenderer::GetRenderpass()
{
	return m_pRenderpassWrapper->GetRenderpass();
}

void DDM3::DeferredRenderer::AddDefaultPipelines()
{
	// Get config manager
	auto& configManager{ ConfigManager::GetInstance() };

	// Initialize default pipeline name 
	auto defaultPipelineName = configManager.GetString("DefaultPipelineName");

	// Add default pipeline
	VulkanObject::GetInstance().AddGraphicsPipeline(defaultPipelineName, {
		configManager.GetString("DefaultDeferredVert"),
		configManager.GetString("DefaultDeferredFrag") });

	// Initialize default pipeline name 
	//auto lightingPipelineName = configManager.GetString("DeferredLighting");
	//
	//// Add default pipeline
	//VulkanObject::GetInstance().AddGraphicsPipeline(lightingPipelineName, {
	//	configManager.GetString("DeferredLightingVert"),
	//	configManager.GetString("DeferredLightingFrag") });
}

void DDM3::DeferredRenderer::InitImgui()
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

void DDM3::DeferredRenderer::CleanupImgui()
{
	m_pImGuiWrapper.reset();
}

void DDM3::DeferredRenderer::RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex)
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

	m_pRenderpassWrapper->BeginRenderPass(commandBuffer, m_pSwapchainWrapper->GetFrameBuffer(imageIndex), extent);

	//DDM3::SceneManager::GetInstance().RenderSkybox();

	SceneManager::GetInstance().Render();

	// Render the ImGui
	//m_pImGuiWrapper->Render(commandBuffer);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void DDM3::DeferredRenderer::RecreateSwapChain()
{
}

void DDM3::DeferredRenderer::CreateRenderpass()
{
	m_pRenderpassWrapper = std::make_unique<RenderpassWrapper>();

	VkSampleCountFlagBits msaaSamples = VulkanObject::GetInstance().GetMsaaSamples();

	auto albedoAttachment = std::make_unique<Attachment>();

	albedoAttachment->SetClearColorValue({ 0.388f, 0.588f, 0.929f, 1.0f });
	albedoAttachment->SetFormat(VK_FORMAT_R8G8B8A8_UNORM);

	// Create attachment description
	VkAttachmentDescription albedoAttachmentDesc{};
	// et format to color format 
	albedoAttachmentDesc.format = VK_FORMAT_R8G8B8A8_UNORM;
	// Give max amount of samplples
	albedoAttachmentDesc.samples = msaaSamples;
	// Set loadOp function to load op clear
	albedoAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// Set storeOp functoin to store op store
	albedoAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Set initial layout to undefined
	albedoAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Set final layout to color attachment optimal
	albedoAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;;

	// Create attachment reference
	VkAttachmentReference albedoAttachmentRef{};
	// Set layout to color attachment optimal
	albedoAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	albedoAttachment->SetAttachmentDesc(albedoAttachmentDesc);

	albedoAttachment->SetAttachmentRef(albedoAttachmentRef);

	m_pRenderpassWrapper->AddAttachment(std::move(albedoAttachment));


	auto normalAttachment = std::make_unique<Attachment>();

	normalAttachment->SetFormat(VK_FORMAT_R16G16B16A16_SFLOAT);

	// Create attachment description
	VkAttachmentDescription normalAttachmentDesc{};
	// et format to color format 
	normalAttachmentDesc.format = VK_FORMAT_R16G16B16A16_SFLOAT;
	// Give max amount of samplples
	normalAttachmentDesc.samples = msaaSamples;
	// Set loadOp function to load op clear
	normalAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// Set storeOp functoin to store op store
	normalAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Set initial layout to undefined
	normalAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Set final layout to color attachment optimal
	normalAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;;

	// Create attachment reference
	VkAttachmentReference normalAttachmentRef{};
	// Set layout to color attachment optimal
	normalAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	normalAttachment->SetAttachmentDesc(normalAttachmentDesc);

	normalAttachment->SetAttachmentRef(normalAttachmentRef);

	m_pRenderpassWrapper->AddAttachment(std::move(normalAttachment));


	auto positionAttachment = std::make_unique<Attachment>();

	positionAttachment->SetFormat(VK_FORMAT_R16G16B16A16_SFLOAT);
	
	// Create attachment description
	VkAttachmentDescription positionAttachmentDesc{};
	// et format to color format 
	positionAttachmentDesc.format = VK_FORMAT_R16G16B16A16_SFLOAT;
	// Give max amount of samplples
	positionAttachmentDesc.samples = msaaSamples;
	// Set loadOp function to load op clear
	positionAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// Set storeOp functoin to store op store
	positionAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Set initial layout to undefined
	positionAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Set final layout to color attachment optimal
	positionAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;;
	
	// Create attachment reference
	VkAttachmentReference positionAttachmentRef{};
	// Set layout to color attachment optimal
	positionAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	positionAttachment->SetAttachmentDesc(positionAttachmentDesc);

	positionAttachment->SetAttachmentRef(positionAttachmentRef);

	m_pRenderpassWrapper->AddAttachment(std::move(positionAttachment));


	auto depthAttachment = std::make_unique<Attachment>();

	depthAttachment->SetFormat(VulkanUtils::FindDepthFormat(VulkanObject::GetInstance().GetPhysicalDevice()));

	VkAttachmentDescription depthAttachmentDesc;
	depthAttachmentDesc.flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
	// Set format to depth format
	depthAttachmentDesc.format = VulkanUtils::FindDepthFormat(VulkanObject::GetInstance().GetPhysicalDevice());
	// Give max amount of samples
	depthAttachmentDesc.samples = VulkanObject::GetInstance().GetMsaaSamples();
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

	VkAttachmentReference depthAttachmentRef{};
	// Set layout to depth stencil attachment optimal
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	depthAttachment->SetAttachmentDesc(depthAttachmentDesc);

	depthAttachment->SetAttachmentRef(depthAttachmentRef);

	m_pRenderpassWrapper->AddDepthAttachment(std::move(depthAttachment));
	
	m_pRenderpassWrapper->CreateRenderPass();
}
