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
#include "Vulkan/VulkanWrappers/PipelineWrapper.h"
#include "DataTypes/DescriptorObjects/TextureDescriptorObject.h"

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

	CreateDepthRenderpass();

	CreateGeometryRenderpass();

	CreateLightingRendepass(m_pSwapchainWrapper->GetFormat());

	auto commandBuffer = VulkanObject::GetInstance().BeginSingleTimeCommands();
	m_pSwapchainWrapper->SetupImageViews(pGPUObject, VulkanObject::GetInstance().GetImageManager(), commandBuffer, m_pDepthRenderpass.get());
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);

	commandBuffer = VulkanObject::GetInstance().BeginSingleTimeCommands();
	m_pSwapchainWrapper->SetupImageViews(pGPUObject, VulkanObject::GetInstance().GetImageManager(), commandBuffer, m_pGeometryRenderpass.get());
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);

	commandBuffer = VulkanObject::GetInstance().BeginSingleTimeCommands();
	m_pSwapchainWrapper->SetupImageViews(pGPUObject, VulkanObject::GetInstance().GetImageManager(), commandBuffer, m_pLightingRenderpass.get());
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);

	// Initialize the sync objects
	m_pSyncObjectManager = std::make_unique<SyncObjectManager>(pGPUObject->GetDevice(), VulkanObject::GetInstance().GetMaxFrames());

	//InitImgui();

	SetupDescriptorSets();
}

DDM3::DeferredRenderer::~DeferredRenderer()
{
	auto device = VulkanObject::GetInstance().GetDevice();

	vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);

	vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
}

void DDM3::DeferredRenderer::Render()
{
	auto& vulkanObject{ DDM3::VulkanObject::GetInstance() };

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

DDM3::RenderpassWrapper* DDM3::DeferredRenderer::GetDefaultRenderpass()
{
	return m_pGeometryRenderpass.get();
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
		configManager.GetString("DefaultDeferredFrag") },
		true, false, m_pGeometryRenderpass.get());

	// Initialize default pipeline name 
	auto lightingPipelineName = configManager.GetString("DeferredLightingPipelineName");
	
	// Add default pipeline
	VulkanObject::GetInstance().AddGraphicsPipeline(lightingPipelineName, {
		configManager.GetString("DeferredLightingVert"),
		configManager.GetString("DeferredLightingFrag") },
		false, true, m_pLightingRenderpass.get());

	m_PipelineLayout = VulkanObject::GetInstance().GetPipeline(lightingPipelineName)->GetPipelineLayout();

	VulkanObject::GetInstance().AddGraphicsPipeline("Depth", {
		"Resources/DefaultResources/Depth.Vert.spv"},
		true, true, m_pDepthRenderpass.get());
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
		VulkanObject::GetInstance().GetDevice(), static_cast<uint32_t>(VulkanObject::GetInstance().GetMaxFrames()), m_pGeometryRenderpass->GetRenderpass(), commandBuffer);
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

	m_pDepthRenderpass->BeginRenderPass(commandBuffer, m_pSwapchainWrapper->GetFrameBuffer(imageIndex, m_pDepthRenderpass.get()), extent);

	SceneManager::GetInstance().RenderDepth();

	vkCmdEndRenderPass(commandBuffer);



	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	barrier.image = m_pDepthTexture->image; // Your actual VkImage
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,   // When depth write completes
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,  // Before depth read starts
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);



	m_pGeometryRenderpass->BeginRenderPass(commandBuffer, m_pSwapchainWrapper->GetFrameBuffer(imageIndex, m_pGeometryRenderpass.get()), extent, false);

	//DDM3::SceneManager::GetInstance().RenderSkybox();

	SceneManager::GetInstance().Render();

	vkCmdEndRenderPass(commandBuffer);

	TransitionImages(commandBuffer, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	
	auto frameBuffer = m_pSwapchainWrapper->GetFrameBuffer(imageIndex, m_pLightingRenderpass.get());

	m_pLightingRenderpass->BeginRenderPass(commandBuffer, frameBuffer, extent);


	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		VulkanObject::GetInstance().GetPipeline(ConfigManager::GetInstance().GetString("DeferredLightingPipelineName"))->GetPipeline());

	BindDescriptorSets(commandBuffer);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	TransitionImages(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);



	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void DDM3::DeferredRenderer::RecreateSwapChain()
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


	std::vector<RenderpassWrapper*> renderpasses{ m_pDepthRenderpass.get(), m_pGeometryRenderpass.get(), m_pLightingRenderpass.get()};

	// Recreate the swapchain
	m_pSwapchainWrapper->RecreateSwapChain(DDM3::VulkanObject::GetInstance().GetGPUObject(), DDM3::VulkanObject::GetInstance().GetSurface(),
		VulkanObject::GetInstance().GetImageManager(),
		commandBuffer, renderpasses);

	// End single time command
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);

	CreateDescriptorSets();
}

void DDM3::DeferredRenderer::CreateDepthRenderpass()
{
	m_pDepthRenderpass = std::make_unique<RenderpassWrapper>();

	m_pDepthRenderpass->SetSampleCount(VK_SAMPLE_COUNT_1_BIT);

	VkSampleCountFlagBits msaaSamples = VulkanObject::GetInstance().GetMsaaSamples();

	auto depthAttachment = std::make_unique<Attachment>();

	depthAttachment->SetFormat(VulkanUtils::FindDepthFormat(VulkanObject::GetInstance().GetPhysicalDevice()));

	VkAttachmentDescription depthAttachmentDesc{};
	depthAttachmentDesc.flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
	// Set format to depth format
	depthAttachmentDesc.format = VulkanUtils::FindDepthFormat(VulkanObject::GetInstance().GetPhysicalDevice());
	// Give max amount of samples
	depthAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	// Set loadOp function to load op clear
	depthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// Set storeOp function to store op don't care
	depthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Set stencilLoadOp function to load op don't care
	depthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	// Set store op function to store op don't care
	depthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Set initial layout to undefined
	depthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Set final layout to depth stencil attachment optimal
	depthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	// Set layout to depth stencil attachment optimal
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	depthAttachment->SetAttachmentDesc(depthAttachmentDesc);

	//depthAttachment->SetAttachmentRef(depthAttachmentRef);

	m_pDepthTexture = depthAttachment->GetTextureSharedPtr();

	//m_pDepthRenderpass->AddDepthAttachment(std::move(depthAttachment));

	m_pDepthRenderpass->CreateRenderPass();

	m_pSwapchainWrapper->AddFrameBuffers(m_pDepthRenderpass.get());

}

void DDM3::DeferredRenderer::CreateGeometryRenderpass()
{
	m_pGeometryRenderpass = std::make_unique<RenderpassWrapper>();

	m_pGeometryRenderpass->SetSampleCount(VK_SAMPLE_COUNT_1_BIT);

	VkSampleCountFlagBits msaaSamples = VulkanObject::GetInstance().GetMsaaSamples();

	auto albedoAttachment = std::make_unique<Attachment>();

	albedoAttachment->SetClearColorValue({ 0.388f, 0.588f, 0.929f, 1.0f });
	albedoAttachment->SetFormat(VK_FORMAT_R8G8B8A8_UNORM);

	// Create attachment description
	VkAttachmentDescription albedoAttachmentDesc{};
	// et format to color format 
	albedoAttachmentDesc.format = VK_FORMAT_R8G8B8A8_UNORM;
	// Give max amount of samplples
	albedoAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
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

	//albedoAttachment->SetAttachmentRef(albedoAttachmentRef);

	m_pGeometryRenderpass->AddAttachment(std::move(albedoAttachment));


	auto normalAttachment = std::make_unique<Attachment>();

	normalAttachment->SetFormat(VK_FORMAT_R16G16B16A16_SFLOAT);

	// Create attachment description
	VkAttachmentDescription normalAttachmentDesc{};
	// et format to color format 
	normalAttachmentDesc.format = VK_FORMAT_R16G16B16A16_SFLOAT;
	// Give max amount of samplples
	normalAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
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

	//normalAttachment->SetAttachmentRef(normalAttachmentRef);

	m_pGeometryRenderpass->AddAttachment(std::move(normalAttachment));


	auto positionAttachment = std::make_unique<Attachment>();

	positionAttachment->SetFormat(VK_FORMAT_R16G16B16A16_SFLOAT);
	
	// Create attachment description
	VkAttachmentDescription positionAttachmentDesc{};
	// et format to color format 
	positionAttachmentDesc.format = VK_FORMAT_R16G16B16A16_SFLOAT;
	// Give max amount of samplples
	positionAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
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

	//positionAttachment->SetAttachmentRef(positionAttachmentRef);

	m_pGeometryRenderpass->AddAttachment(std::move(positionAttachment));


	auto depthAttachment = std::make_unique<Attachment>();

	depthAttachment->SetFormat(VulkanUtils::FindDepthFormat(VulkanObject::GetInstance().GetPhysicalDevice()));

	VkAttachmentDescription depthAttachmentDesc{};
	depthAttachmentDesc.flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
	// Set format to depth format
	depthAttachmentDesc.format = VulkanUtils::FindDepthFormat(VulkanObject::GetInstance().GetPhysicalDevice());
	// Give max amount of samples
	depthAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	// Set loadOp function to load op clear
	depthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	// Set storeOp function to store op don't care
	depthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// Set stencilLoadOp function to load op don't care
	depthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	// Set store op function to store op don't care
	depthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// Set initial layout to undefined
	depthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	// Set final layout to depth stencil attachment optimal
	depthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	// Set layout to depth stencil attachment optimal
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	depthAttachment->SetAttachmentDesc(depthAttachmentDesc);

	//depthAttachment->SetAttachmentRef(depthAttachmentRef);

	depthAttachment->SetTexture(m_pDepthTexture);

	//m_pGeometryRenderpass->AddDepthAttachment(std::move(depthAttachment));
	
	m_pGeometryRenderpass->CreateRenderPass();

	m_pSwapchainWrapper->AddFrameBuffers(m_pGeometryRenderpass.get());
}

void DDM3::DeferredRenderer::CreateLightingRendepass(VkFormat swapchainFormat)
{
	m_pLightingRenderpass = std::make_unique<RenderpassWrapper>();

	auto& vulkanObject = VulkanObject::GetInstance();

	VkSampleCountFlagBits msaaSamples = vulkanObject.GetMsaaSamples(); //VK_SAMPLE_COUNT_1_BIT;


	m_pLightingRenderpass->SetSampleCount(msaaSamples);

	std::unique_ptr<Attachment> attachment = std::make_unique<Attachment>();

	attachment->SetFormat(swapchainFormat);
	attachment->SetClearColorValue({ 0.388f, 0.588f, 0.929f, 1.0f });

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

	attachment->SetAttachmentDesc(colorAttachment);
	//attachment->SetAttachmentRef(colorAttachmentRef);

	m_pLightingRenderpass->AddAttachment(std::move(attachment));


	auto depthAttachment = std::make_unique<Attachment>();

	depthAttachment->SetFormat(VulkanUtils::FindDepthFormat(vulkanObject.GetPhysicalDevice()));

	VkAttachmentDescription depthAttachmentDesc{};
	depthAttachmentDesc.flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
	// Set format to depth format
	depthAttachmentDesc.format = VulkanUtils::FindDepthFormat(vulkanObject.GetPhysicalDevice());
	// Give max amount of samples
	depthAttachmentDesc.samples = msaaSamples;
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

	//depthAttachment->SetAttachmentRef(depthAttachmentRef);

	//m_pLightingRenderpass->AddDepthAttachment(std::move(depthAttachment));



	auto colorResolveAttachment = std::make_unique<Attachment>();

	colorResolveAttachment->SetFormat(swapchainFormat);

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


	VkAttachmentReference colorResolveRef{};
	// Set layout to color attachment optimal
	colorResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	colorResolveAttachment->SetAttachmentDesc(colorResolveDesc);

	//colorResolveAttachment->SetAttachmentRef(colorResolveRef);

	//m_pLightingRenderpass->AddColorResolveAttachment(std::move(colorResolveAttachment));

	m_pLightingRenderpass->CreateRenderPass();

	m_pSwapchainWrapper->AddFrameBuffers(m_pLightingRenderpass.get());
}

void DDM3::DeferredRenderer::CreateDescriptorSetLayout()
{
	// Create vector of descriptorsetlayoutbindings the size of the sum of vertexUbos, fragmentUbos and textureamount;
	std::vector<VkDescriptorSetLayoutBinding> bindings{};

	// Add the descriptor layout bindings for each shader module
	for (int i{}; i < 3; ++i)
	{
		VkDescriptorSetLayoutBinding binding{};

		binding.binding = i;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.descriptorCount = 1;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		binding.pImmutableSamplers = nullptr;

		bindings.push_back(binding);
	}

	// Create layout info
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	// Set type to descriptor set layout create info
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	// Set bindingcount to the amount of bindings
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	// Set bindings to the data of bindings vector
	layoutInfo.pBindings = bindings.data();

	// Create descriptorset layout
	if (vkCreateDescriptorSetLayout(VulkanObject::GetInstance().GetDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
	{
		// If not successfull, throw runtime error
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void DDM3::DeferredRenderer::CreateDescriptorPool()
{
	// Get a reference to the renderer
	auto& vulkanObject{ VulkanObject::GetInstance() };

	// Get the amount of frames in flight
	auto frames{ vulkanObject.GetMaxFrames() };

	// Create a vector of pool sizes
	std::vector<VkDescriptorPoolSize> poolSizes{};

	// Loop trough all the descriptor types
	for (int i{}; i < 3; ++i)
	{
		VkDescriptorPoolSize descriptorPoolSize{};
		// Set the type of the poolsize
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		// Calculate the amount of descriptors needed from this type
		descriptorPoolSize.descriptorCount = frames;

		// Add the descriptor poolsize to the vector
		poolSizes.push_back(descriptorPoolSize);
	}

	// Create pool info
	VkDescriptorPoolCreateInfo poolInfo{};
	// Set type to descriptor pool create info
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	// Set sizecount to the size of poolsizes
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	// Give poolSizes
	poolInfo.pPoolSizes = poolSizes.data();
	// Give max sets
	poolInfo.maxSets = static_cast<uint32_t>(frames);
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	// Created descriptorpool, if not successful, throw runtime error
	if (vkCreateDescriptorPool(vulkanObject.GetDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void DDM3::DeferredRenderer::CreateDescriptorSets()
{
	m_TextureDescriptorObjects.clear();

	if (m_DescriptorSets.size() > 0)
	{
		vkFreeDescriptorSets(VulkanObject::GetInstance().GetDevice(), m_DescriptorPool, m_DescriptorSets.size(), m_DescriptorSets.data());
	}

	m_DescriptorSets.clear();
	m_DescriptorSets.resize(VulkanObject::GetInstance().GetMaxFrames());

	auto& attachmentList = m_pGeometryRenderpass->GetAttachmentList();

	for (auto& attachment : attachmentList)
	{
		auto descriptorObject = std::make_unique<TextureDescriptorObject>();
		descriptorObject->AddTextures(*attachment->GetTexture());
		descriptorObject->SetCleanupTextures(false);

		m_TextureDescriptorObjects.push_back(std::move(descriptorObject));
	}


	// Get a reference to the renderer for later use
	auto& renderer{ VulkanObject::GetInstance() };

	// Get the amount of frames in flight
	auto maxFrames = renderer.GetMaxFrames();

	// Create the allocation info for the descriptorsets
	VkDescriptorSetAllocateInfo allocInfo{};
	// Set type to descriptor set allocate info
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	// Give the right descriptorpool
	allocInfo.descriptorPool = m_DescriptorPool;
	// Give the amount of descriptorsets to be allocated
	allocInfo.descriptorSetCount = static_cast<uint32_t>(maxFrames);
	// Create vector of descriptorsets the size of maxFrames and fill with layout
	std::vector<VkDescriptorSetLayout> layouts(maxFrames, m_DescriptorSetLayout);
	allocInfo.pSetLayouts = layouts.data();


	// Allocate descriptorsets, if not succeeded, throw runtime error
	if (vkAllocateDescriptorSets(renderer.GetDevice(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	UpdateDescriptorSets();
}

void DDM3::DeferredRenderer::SetupDescriptorSets()
{
	CreateDescriptorSetLayout();

	CreateDescriptorPool();

	CreateDescriptorSets();
}

void DDM3::DeferredRenderer::UpdateDescriptorSets()
{
	std::vector<VkWriteDescriptorSet> descriptorWrites{};

	// Loop trough all the descriptor sets
	for (int i{}; i < static_cast<int>(m_DescriptorSets.size()); i++)
	{
		// Create a vector of descriptor writes
		std::vector<VkWriteDescriptorSet> descriptorWrites{};

		// Initialize current binding with 0
		int binding{};

		// Loop trough all descriptor objects and add the descriptor writes
		for (auto& descriptorObject : m_TextureDescriptorObjects)
		{
			descriptorObject->AddDescriptorWrite(m_DescriptorSets[i], descriptorWrites, binding, 1, i);
		}


		//vkDeviceWaitIdle(VulkanRenderer::GetInstance().GetDevice());
		// Update descriptorsets
		vkUpdateDescriptorSets(VulkanObject::GetInstance().GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void DDM3::DeferredRenderer::BindDescriptorSets(VkCommandBuffer commandBuffer)
{
	auto& vulkanObject = VulkanObject::GetInstance();
	
	auto frame = vulkanObject.GetCurrentFrame();

	// Bind descriptor sets
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[frame], 0, nullptr);
}

void DDM3::DeferredRenderer::TransitionImages(VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	std::vector<VkImageMemoryBarrier> barriers;
	VkPipelineStageFlags srcStage{};
	VkPipelineStageFlags dstStage{};

	for (auto& attachment : m_pGeometryRenderpass->GetAttachmentList()) {
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = attachment->GetTexture()->image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
			newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
			newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else {
			throw std::runtime_error("Unsupported layout transition!");
		}

		barriers.push_back(barrier);
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		srcStage,
		dstStage,
		0,
		0, nullptr,
		0, nullptr,
		static_cast<uint32_t>(barriers.size()), barriers.data()
	);
}
