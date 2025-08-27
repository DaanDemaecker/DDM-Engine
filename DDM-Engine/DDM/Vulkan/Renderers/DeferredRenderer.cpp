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
#include "Vulkan/VulkanWrappers/Subpass.h"

#include "Engine/Window.h"
#include "Managers/SceneManager.h"

#include "Managers/ConfigManager.h"

DDM::DeferredRenderer::DeferredRenderer()
{
	auto surface{ VulkanObject::GetInstance().GetSurface() };

	// Get pointer to gpu object
	GPUObject* pGPUObject{ VulkanObject::GetInstance().GetGPUObject() };

	// Initialize the swapchain
	m_pSwapchainWrapper = std::make_unique<SwapchainWrapper>(pGPUObject, surface, VulkanObject::GetInstance().GetImageManager(), VulkanObject::GetInstance().GetMsaaSamples());

	//CreateMasterRenderpass();

	CreateRenderpass();

	auto commandBuffer = VulkanObject::GetInstance().BeginSingleTimeCommands();
	m_pSwapchainWrapper->SetupImageViews(pGPUObject, VulkanObject::GetInstance().GetImageManager(), commandBuffer, m_pRenderpass.get());
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);


	SetupDescriptorObjects();

	CreateDescriptorPool();
	
	CreateDescriptorSetLayout();

	CreateDescriptorSets();



	// Initialize the sync objects
	m_pSyncObjectManager = std::make_unique<SyncObjectManager>(pGPUObject->GetDevice(), VulkanObject::GetInstance().GetMaxFrames());

	InitImgui();
}

DDM::DeferredRenderer::~DeferredRenderer()
{
	auto device = VulkanObject::GetInstance().GetDevice();

	vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);

	vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
}

void DDM::DeferredRenderer::Render()
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

VkExtent2D DDM::DeferredRenderer::GetExtent()
{
	return m_pSwapchainWrapper->GetExtent();
}

DDM::RenderpassWrapper* DDM::DeferredRenderer::GetDefaultRenderpass()
{
	return m_pRenderpass.get();
}

void DDM::DeferredRenderer::AddDefaultPipelines()
{
	// Get config manager
	auto& configManager{ ConfigManager::GetInstance() };


	VulkanObject::GetInstance().AddGraphicsPipeline(configManager.GetString("DepthPipelineName"), {
		configManager.GetString("DepthVert")});


	// Initialize default pipeline name 
	auto defaultPipelineName = configManager.GetString("DefaultPipelineName");

	// Add default pipeline
	VulkanObject::GetInstance().AddGraphicsPipeline(defaultPipelineName, {
		configManager.GetString("DefaultDeferredVert"),
		configManager.GetString("DefaultDeferredFrag") },
		true, false, kSubpass_GBUFFER);

	// Initialize default pipeline name 
	auto lightingPipelineName = configManager.GetString("DeferredLightingPipelineName");
	
	// Add default pipeline
	VulkanObject::GetInstance().AddGraphicsPipeline(lightingPipelineName, {
		configManager.GetString("DrawQuadVert"),
		configManager.GetString("DeferredLightingFrag") },
		false, true, kSubpass_LIGHTING);

	m_pLightingPipeline = VulkanObject::GetInstance().GetPipeline(lightingPipelineName);
}

void DDM::DeferredRenderer::CreateRenderpass()
{
	m_pRenderpass = std::make_unique<RenderpassWrapper>();

	SetupAttachments();

	SetupDepthPass();

	SetupGeometryPass();

	SetupLightingPass();

	SetupImGuiPass();

	SetupDependencies();

	m_pRenderpass->CreateRenderPass();

	m_pSwapchainWrapper->AddFrameBuffers(m_pRenderpass.get());

}

void DDM::DeferredRenderer::SetupAttachments()
{
	int swapchainImageAmount = m_pSwapchainWrapper->GetSwapchainImageAmount();

	// Set up backbuffer attachment
	auto backbufferFormat = m_pSwapchainWrapper->GetFormat();

	auto backBufferAttachment = std::make_unique<Attachment>(swapchainImageAmount);
	backBufferAttachment->SetIsSwapchainImage(true);
	backBufferAttachment->SetClearColorValue({ 0.388f, 0.588f, 0.929f, 1.0f });
	backBufferAttachment->SetFormat(backbufferFormat);
	backBufferAttachment->SetAttachmentType(Attachment::kAttachmentType_Color);


	VkAttachmentDescription backBufferAttachmentDesc{};
	backBufferAttachmentDesc.flags = 0;
	backBufferAttachmentDesc.format = backbufferFormat;
	backBufferAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	backBufferAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	backBufferAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	backBufferAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	backBufferAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	backBufferAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	backBufferAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	backBufferAttachment->SetAttachmentDesc(backBufferAttachmentDesc);

	// Set up Depth attachment
	auto depthFormat = VulkanUtils::FindDepthFormat(VulkanObject::GetInstance().GetPhysicalDevice());
	auto depthAttachment = std::make_unique<Attachment>(swapchainImageAmount);

	depthAttachment->SetAttachmentType(Attachment::kAttachmentType_DepthStencil);
	depthAttachment->SetFormat(depthFormat);
	depthAttachment->SetIsInput(true);

	VkAttachmentDescription depthAttachmentDesc{};
	depthAttachmentDesc.flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
	// Set format to depth format
	depthAttachmentDesc.format = depthFormat;
	// Give max amount of samples
	depthAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	// Set loadOp function to load op clear
	depthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// Set storeOp function to store op don't care
	depthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Set stencilLoadOp function to load op don't care
	depthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// Set store op function to store op don't care
	depthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Set initial layout to undefined
	depthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Set final layout to depth stencil attachment optimal
	depthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	depthAttachment->SetAttachmentDesc(depthAttachmentDesc);




	// Set up color attachments
	auto colorAttachmentFormat = VK_FORMAT_R8G8B8A8_UNORM;

	// albedo attachment
	auto albedoAttachment = std::make_unique<Attachment>(swapchainImageAmount);
	albedoAttachment->SetClearColorValue({ 0.388f, 0.588f, 0.929f, 1.0f });
	albedoAttachment->SetFormat(colorAttachmentFormat);
	albedoAttachment->SetAttachmentType(Attachment::kAttachmentType_Color);
	albedoAttachment->SetIsInput(true);


	VkAttachmentDescription albedoAttachmentDesc{};
	albedoAttachmentDesc.flags = 0;
	albedoAttachmentDesc.format = colorAttachmentFormat;
	albedoAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	albedoAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	albedoAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	albedoAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	albedoAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	albedoAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	albedoAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	albedoAttachment->SetAttachmentDesc(albedoAttachmentDesc);

	// normal attachment
	auto normalAttachment = std::make_unique<Attachment>(swapchainImageAmount);
	normalAttachment->SetFormat(colorAttachmentFormat);
	normalAttachment->SetAttachmentType(Attachment::kAttachmentType_Color);
	normalAttachment->SetIsInput(true);


	VkAttachmentDescription normalAttachmentDesc{};
	normalAttachmentDesc.flags = 0;
	normalAttachmentDesc.format = colorAttachmentFormat;
	normalAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	normalAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	normalAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	normalAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	normalAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	normalAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	normalAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	
	normalAttachment->SetAttachmentDesc(normalAttachmentDesc);

	// posiition attachment
	auto posiitionAttachment = std::make_unique<Attachment>(swapchainImageAmount);
	posiitionAttachment->SetFormat(colorAttachmentFormat);
	posiitionAttachment->SetAttachmentType(Attachment::kAttachmentType_Color);
	posiitionAttachment->SetIsInput(true);


	VkAttachmentDescription posiitionAttachmentDesc{};
	posiitionAttachmentDesc.flags = 0;
	posiitionAttachmentDesc.format = colorAttachmentFormat;
	posiitionAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	posiitionAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	posiitionAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	posiitionAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	posiitionAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	posiitionAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	posiitionAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	posiitionAttachment->SetAttachmentDesc(posiitionAttachmentDesc);


	m_pRenderpass->AddAttachment(std::move(backBufferAttachment));
	m_pRenderpass->AddAttachment(std::move(depthAttachment));
	m_pRenderpass->AddAttachment(std::move(albedoAttachment));
	m_pRenderpass->AddAttachment(std::move(normalAttachment));
	m_pRenderpass->AddAttachment(std::move(posiitionAttachment));
}

void DDM::DeferredRenderer::SetupDepthPass()
{
	// Depth prepass depth buffer reference (read/write)
	VkAttachmentReference depthAttachmentReference{};
	depthAttachmentReference.attachment = kAttachment_DEPTH;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::unique_ptr<Subpass> depthPass{std::make_unique<Subpass>()};

	depthPass->AddDepthRef(depthAttachmentReference);

	m_pRenderpass->AddSubpass(std::move(depthPass));
}

void DDM::DeferredRenderer::SetupGeometryPass()
{
	std::unique_ptr<Subpass> pGBufferPass{ std::make_unique<Subpass>() };

	// Depth prepass depth buffer reference (read/write)
	VkAttachmentReference depthAttachmentReference{};
	depthAttachmentReference.attachment = kAttachment_DEPTH;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	pGBufferPass->AddDepthRef(depthAttachmentReference);


	VkAttachmentReference albedoAttachmentRef{};
	albedoAttachmentRef.attachment = kAttachment_GBUFFER_ALBEDO;
	albedoAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	pGBufferPass->AddReference(albedoAttachmentRef);


	VkAttachmentReference normalAttachmentRef{};
	normalAttachmentRef.attachment = kAttachment_GBUFFER_NORMAL;
	normalAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	pGBufferPass->AddReference(normalAttachmentRef);


	VkAttachmentReference positionAttachmentRef{};
	positionAttachmentRef.attachment = kAttachment_GBUFFER_POSITION;
	positionAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	pGBufferPass->AddReference(positionAttachmentRef);


	m_pRenderpass->AddSubpass(std::move(pGBufferPass));
}

void DDM::DeferredRenderer::SetupLightingPass()
{
	std::unique_ptr<Subpass> pLightingPass{ std::make_unique<Subpass>() };

	// Depth prepass depth buffer reference (read/write)
	VkAttachmentReference depthAttachmentReference{};
	depthAttachmentReference.attachment = kAttachment_DEPTH;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

	pLightingPass->AddInputReference(depthAttachmentReference);


	VkAttachmentReference albedoAttachmentRef{};
	albedoAttachmentRef.attachment = kAttachment_GBUFFER_ALBEDO;
	albedoAttachmentRef.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	pLightingPass->AddInputReference(albedoAttachmentRef);


	VkAttachmentReference normalAttachmentRef{};
	normalAttachmentRef.attachment = kAttachment_GBUFFER_NORMAL;
	normalAttachmentRef.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	pLightingPass->AddInputReference(normalAttachmentRef);


	VkAttachmentReference positionAttachmentRef{};
	positionAttachmentRef.attachment = kAttachment_GBUFFER_POSITION;
	positionAttachmentRef.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	pLightingPass->AddInputReference(positionAttachmentRef);

	// Final pass-back buffer render reference
	VkAttachmentReference backBufferRenderRef{};
	backBufferRenderRef.attachment = kAttachment_BACK;
	backBufferRenderRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	pLightingPass->AddReference(backBufferRenderRef);

	m_pRenderpass->AddSubpass(std::move(pLightingPass));
}

void DDM::DeferredRenderer::SetupImGuiPass()
{
	int swapchainImageAmount = m_pSwapchainWrapper->GetSwapchainImageAmount();

	// Create subpass
	std::unique_ptr<Subpass> imGuiSubpass{ std::make_unique<Subpass>() };


	// Final pass-back buffer render reference
	VkAttachmentReference backBufferRenderRef{};
	backBufferRenderRef.attachment = kAttachment_BACK;
	backBufferRenderRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	imGuiSubpass->AddReference(backBufferRenderRef);

	m_pRenderpass->AddSubpass(std::move(imGuiSubpass));
}

void DDM::DeferredRenderer::SetupDependencies()
{
	VkSubpassDependency dependency1{};
	dependency1.srcSubpass = kSubpass_DEPTH;
	dependency1.dstSubpass = kSubpass_GBUFFER;
	dependency1.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency1.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependency1.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency1.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependency1.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkSubpassDependency dependency2{};
	dependency2.srcSubpass = kSubpass_GBUFFER;
	dependency2.dstSubpass = kSubpass_LIGHTING;
	dependency2.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency2.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependency2.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency2.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependency2.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkSubpassDependency dependency3{};
	dependency3.srcSubpass = kSubpass_LIGHTING;
	dependency3.dstSubpass = kSubpass_IMGUI;
	dependency3.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency3.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependency3.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency3.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependency3.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	m_pRenderpass->AddDependency(dependency1);
	m_pRenderpass->AddDependency(dependency2);
}

void DDM::DeferredRenderer::SetupDescriptorObjects()
{
	m_pInputAttachmentList.clear();

	auto& attachments{ m_pRenderpass->GetAttachmentList() };

	for (int i{ kAttachment_GBUFFER_ALBEDO }; i <= kAttachment_GBUFFER_POSITION; ++i)
	{
		auto descriptorObject{ std::make_unique<InputAttachmentDescriptorObject>() };

		descriptorObject->AddImageView(attachments[i]->GetTexture(0)->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		m_pInputAttachmentList.push_back(std::move(descriptorObject));
	}
}

void DDM::DeferredRenderer::InitImgui()
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
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Subpass = kSubpass_IMGUI;

	// Create a single time command buffer
	auto commandBuffer{ VulkanObject::GetInstance().BeginSingleTimeCommands() };
	// Initialize ImGui
	m_pImGuiWrapper = std::make_unique<DDM::ImGuiWrapper>(init_info,
		VulkanObject::GetInstance().GetDevice(), static_cast<uint32_t>(VulkanObject::GetInstance().GetMaxFrames()), m_pRenderpass->GetRenderpass(), commandBuffer);
	// End the single time command buffer
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);
}

void DDM::DeferredRenderer::CleanupImgui()
{
	m_pImGuiWrapper.reset();
}

void DDM::DeferredRenderer::RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex)
{
	auto frame = VulkanObject::GetInstance().GetCurrentFrame();

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
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


	SceneManager::GetInstance().RenderDepth();


	vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

	SceneManager::GetInstance().Render();

	SceneManager::GetInstance().RenderTransparancy();


	vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pLightingPipeline->GetPipeline());

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pLightingPipeline->GetPipelineLayout(), 0, 1,
		&m_DescriptorSets[frame], 0, nullptr);

	VulkanObject::GetInstance().DrawQuad(commandBuffer);

	vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

	// Render the ImGui
	m_pImGuiWrapper->Render(commandBuffer);

	vkCmdEndRenderPass(commandBuffer);

	VkImage swapchainImage = m_pSwapchainWrapper->GetSwapchainImage(imageIndex);

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	barrier.dstAccessMask = 0;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = swapchainImage;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);


	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void DDM::DeferredRenderer::RecreateSwapChain()
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

	ResetDescriptorSets();
}

void DDM::DeferredRenderer::ResetDescriptorSets()
{
	vkFreeDescriptorSets(VulkanObject::GetInstance().GetDevice(), m_DescriptorPool, m_DescriptorSets.size(), m_DescriptorSets.data());

	SetupDescriptorObjects();

	CreateDescriptorSets();
}

void DDM::DeferredRenderer::CreateDescriptorSetLayout()
{
	// Create vector of descriptorsetlayoutbindings the size of the sum of vertexUbos, fragmentUbos and textureamount;
	std::vector<VkDescriptorSetLayoutBinding> bindings{};

	// Add the descriptor layout bindings for each shader module
	for (int i{}; i < 3; ++i)
	{
		VkDescriptorSetLayoutBinding binding{};

		binding.binding = i;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
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

void DDM::DeferredRenderer::CreateDescriptorPool()
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
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
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

void DDM::DeferredRenderer::CreateDescriptorSets()
{
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

	// Resize descriptorsets to right amount
	m_DescriptorSets.resize(maxFrames);

	// Allocate descriptorsets, if not succeeded, throw runtime error
	if (vkAllocateDescriptorSets(renderer.GetDevice(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}



	// Loop trough all the descriptor sets
	for (int i{}; i < static_cast<int>(m_DescriptorSets.size()); i++)
	{
		// Create a vector of descriptor writes
		std::vector<VkWriteDescriptorSet> descriptorWrites{};

		// Initialize current binding with 0
		int binding{};

		// Loop trough all descriptor objects and add the descriptor writes
		for (auto& descriptorObject : m_pInputAttachmentList)
		{
			descriptorObject->AddDescriptorWrite(m_DescriptorSets[i], descriptorWrites, binding, 1, i);
		}


		//vkDeviceWaitIdle(VulkanRenderer::GetInstance().GetDevice());
		// Update descriptorsets
		vkUpdateDescriptorSets(VulkanObject::GetInstance().GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}
