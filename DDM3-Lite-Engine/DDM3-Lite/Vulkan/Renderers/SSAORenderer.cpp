// SSAORenderer.cpp

// Header include 
#include "SSAORenderer.h"

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

#include "Utils/Utils.h"
#include "Components/CameraComponent.h"

DDM3::SSAORenderer::SSAORenderer()
{
	SetupPositionTexture();

	SetupNoiseTexture();

	SetupSamples();

	SetupProjectionMatrix();


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

	CreateDescriptorPools();
	
	CreateDescriptorSetLayouts();

	CreateDescriptorSets();



	// Initialize the sync objects
	m_pSyncObjectManager = std::make_unique<SyncObjectManager>(pGPUObject->GetDevice(), VulkanObject::GetInstance().GetMaxFrames());

	InitImgui();
}

DDM3::SSAORenderer::~SSAORenderer()
{
	auto device = VulkanObject::GetInstance().GetDevice();

	vkDestroyDescriptorSetLayout(device, m_LightingDescriptorSetLayout, nullptr);

	vkDestroyDescriptorPool(device, m_LightingDescriptorPool, nullptr);


	vkDestroyDescriptorSetLayout(device, m_AoBlurDescriptorSetLayout, nullptr);

	vkDestroyDescriptorPool(device, m_AoBlurDescriptorPool, nullptr);


	vkDestroyDescriptorSetLayout(device, m_AoGenDescriptorSetLayout, nullptr);

	vkDestroyDescriptorPool(device, m_AoGenDescriptorPool, nullptr);
}


void DDM3::SSAORenderer::Render()
{
	auto& vulkanObject{ DDM3::VulkanObject::GetInstance() };

	auto currentFrame{ vulkanObject.GetCurrentFrame() };

	auto device{ vulkanObject.GetDevice() };
	auto& queueObject{ vulkanObject.GetQueueObject() };

	vkWaitForFences(device, 1, &m_pSyncObjectManager->GetInFlightFence(currentFrame), VK_TRUE, UINT64_MAX);


	uint32_t imageIndex{};
	VkResult result = vkAcquireNextImageKHR(device, m_pSwapchainWrapper->GetSwapchain(),
		UINT64_MAX, m_pSyncObjectManager->GetImageAvailableSemaphore(currentFrame), VK_NULL_HANDLE, &imageIndex);


	SetNewSamples(currentFrame, imageIndex);

	UpdateAoBlurDescriptorSets(currentFrame, imageIndex);

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

VkExtent2D DDM3::SSAORenderer::GetExtent()
{
	return m_pSwapchainWrapper->GetExtent();
}

DDM3::RenderpassWrapper* DDM3::SSAORenderer::GetDefaultRenderpass()
{
	return m_pRenderpass.get();
}

void DDM3::SSAORenderer::AddDefaultPipelines()
{
	// Get config manager
	auto& configManager{ ConfigManager::GetInstance() };

	// Get reference to vulkan object
	auto& vulkanObject = VulkanObject::GetInstance();


	vulkanObject.AddGraphicsPipeline(configManager.GetString("DepthPipelineName"), {
		configManager.GetString("DepthVert")});


	// Initialize default pipeline name 
	auto defaultPipelineName = configManager.GetString("DefaultPipelineName");

	// Add default pipeline
	vulkanObject.AddGraphicsPipeline(defaultPipelineName, {
		"Resources/Shaders/SSAOGbuffer.vert.spv",
		"Resources/Shaders/SSAOGbuffer.frag.spv" },
		true, false, kSubpass_GBUFFER);

	// Initialize default pipeline name 
	auto lightingPipelineName = configManager.GetString("DeferredLightingPipelineName");
	
	// Add default pipeline
	vulkanObject.AddGraphicsPipeline(lightingPipelineName, {
		configManager.GetString("DrawQuadVert"),
		"Resources/Shaders/SSAOLighting.frag.spv" },
		false, true, kSubpass_LIGHTING);

	m_pLightingPipeline = vulkanObject.GetPipeline(lightingPipelineName);


	auto aoPipelineName = "AoGeneration";

	vulkanObject.AddGraphicsPipeline(aoPipelineName, {
		configManager.GetString("DrawQuadVert"),
		"Resources/Shaders/SSAOGen.frag.spv"},
		true, true, kSubpass_AO_GEN);

	m_pAoPipeline = vulkanObject.GetPipeline(aoPipelineName);



	auto aoBlurPipelineName = "AoBlur";

	vulkanObject.AddGraphicsPipeline(aoBlurPipelineName, {
		configManager.GetString("DrawQuadVert"),
		"Resources/Shaders/SSAOBlur.frag.spv" },
		true, true, kSubpass_AO_BLUR);


	m_pAoBlurPipeline = vulkanObject.GetPipeline(aoBlurPipelineName);
}

void DDM3::SSAORenderer::CreateRenderpass()
{
	m_pRenderpass = std::make_unique<RenderpassWrapper>();

	SetupAttachments();

	SetupDepthPass();

	SetupGBufferPass();

	SetupAoGenPass();

	SetupAoBlurPass();

	SetupLightingPass();

	SetupImGuiPass();

	SetupDependencies();

	m_pRenderpass->CreateRenderPass();

	m_pSwapchainWrapper->AddFrameBuffers(m_pRenderpass.get());
}

void DDM3::SSAORenderer::SetupAttachments()
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
	depthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

	depthAttachment->SetAttachmentDesc(depthAttachmentDesc);




	// Set up color attachments
	auto colorAttachmentFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

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
	auto positionAttachment = std::make_unique<Attachment>(swapchainImageAmount);
	positionAttachment->SetFormat(colorAttachmentFormat);
	positionAttachment->SetAttachmentType(Attachment::kAttachmentType_Color);
	positionAttachment->SetIsInput(true);


	VkAttachmentDescription positionAttachmentDesc{};
	positionAttachmentDesc.flags = 0;
	positionAttachmentDesc.format = colorAttachmentFormat;
	positionAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	positionAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	positionAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	positionAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	positionAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	positionAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	positionAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	positionAttachment->SetAttachmentDesc(positionAttachmentDesc);
	
	// viewspace normal attachment
	auto viewNormalAttachment = std::make_unique<Attachment>(swapchainImageAmount);
	viewNormalAttachment->SetFormat(colorAttachmentFormat);
	viewNormalAttachment->SetAttachmentType(Attachment::kAttachmentType_Color);
	viewNormalAttachment->SetIsInput(true);


	VkAttachmentDescription viewNormalAttachmentDesc{};
	viewNormalAttachmentDesc.flags = 0;
	viewNormalAttachmentDesc.format = colorAttachmentFormat;
	viewNormalAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	viewNormalAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	viewNormalAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	viewNormalAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	viewNormalAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	viewNormalAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	viewNormalAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	viewNormalAttachment->SetAttachmentDesc(viewNormalAttachmentDesc);

	// viewspace posiition attachment
	auto viewPositionAttachment = std::make_unique<Attachment>(swapchainImageAmount);
	viewPositionAttachment->SetFormat(colorAttachmentFormat);
	viewPositionAttachment->SetAttachmentType(Attachment::kAttachmentType_Color);
	viewPositionAttachment->SetIsInput(true);


	VkAttachmentDescription viewPositionAttachmentDesc{};
	viewPositionAttachmentDesc.flags = 0;
	viewPositionAttachmentDesc.format = colorAttachmentFormat;
	viewPositionAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	viewPositionAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	viewPositionAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	viewPositionAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	viewPositionAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	viewPositionAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	viewPositionAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	viewPositionAttachment->SetAttachmentDesc(viewPositionAttachmentDesc);


	VkFormat aoMapFormat = VK_FORMAT_R32_SFLOAT;
	// AoGen attachment
	auto aoMapAttachment = std::make_unique<Attachment>(swapchainImageAmount);
	aoMapAttachment->SetClearColorValue({ 1.0f, 1.0f, 1.0f, 1.0f });
	aoMapAttachment->SetFormat(aoMapFormat);
	aoMapAttachment->SetAttachmentType(Attachment::kAttachmentType_Color);
	aoMapAttachment->SetIsInput(true);

	VkAttachmentDescription aoMapAttachmentDesc{};
	aoMapAttachmentDesc.flags = 0;
	aoMapAttachmentDesc.format = aoMapFormat;
	aoMapAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	aoMapAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	aoMapAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	aoMapAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	aoMapAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	aoMapAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	aoMapAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	aoMapAttachment->SetAttachmentDesc(aoMapAttachmentDesc);


	// AoBlur attachment
	auto aoBlurMapAttachment = std::make_unique<Attachment>(swapchainImageAmount);
	aoBlurMapAttachment->SetClearColorValue({ 1.0f, 1.0f, 1.0f, 1.0f });
	aoBlurMapAttachment->SetFormat(aoMapFormat);
	aoBlurMapAttachment->SetAttachmentType(Attachment::kAttachmentType_Color);
	aoBlurMapAttachment->SetIsInput(true);

	VkAttachmentDescription aoBlurMapAttachmentDesc{};
	aoBlurMapAttachmentDesc.flags = 0;
	aoBlurMapAttachmentDesc.format = aoMapFormat;
	aoBlurMapAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	aoBlurMapAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	aoBlurMapAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	aoBlurMapAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	aoBlurMapAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	aoBlurMapAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	aoBlurMapAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	aoBlurMapAttachment->SetAttachmentDesc(aoBlurMapAttachmentDesc);

	m_pRenderpass->AddAttachment(std::move(backBufferAttachment));
	m_pRenderpass->AddAttachment(std::move(depthAttachment));
	m_pRenderpass->AddAttachment(std::move(albedoAttachment));
	m_pRenderpass->AddAttachment(std::move(normalAttachment));
	m_pRenderpass->AddAttachment(std::move(positionAttachment));
	m_pRenderpass->AddAttachment(std::move(viewNormalAttachment));
	m_pRenderpass->AddAttachment(std::move(viewPositionAttachment));
	m_pRenderpass->AddAttachment(std::move(aoMapAttachment));
	m_pRenderpass->AddAttachment(std::move(aoBlurMapAttachment));

}

void DDM3::SSAORenderer::SetupDepthPass()
{
	// Depth prepass depth buffer reference (read/write)
	VkAttachmentReference depthAttachmentReference{};
	depthAttachmentReference.attachment = kAttachment_DEPTH;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::unique_ptr<Subpass> depthPass{std::make_unique<Subpass>()};

	depthPass->AddDepthRef(depthAttachmentReference);

	m_pRenderpass->AddSubpass(std::move(depthPass));
}

void DDM3::SSAORenderer::SetupGBufferPass()
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

	VkAttachmentReference viewNormalAttachmentRef{};
	viewNormalAttachmentRef.attachment = kAttachment_GBUFFER_VIEWNORMAL;
	viewNormalAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	pGBufferPass->AddReference(viewNormalAttachmentRef);


	VkAttachmentReference viewPositionAttachmentRef{};
	viewPositionAttachmentRef.attachment = kAttachment_GBUFFER_VIEWPOS;
	viewPositionAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	pGBufferPass->AddReference(viewPositionAttachmentRef);


	m_pRenderpass->AddSubpass(std::move(pGBufferPass));
}

void DDM3::SSAORenderer::SetupAoGenPass()
{
	std::unique_ptr<Subpass> pAoMapPass{ std::make_unique<Subpass>() };

	VkAttachmentReference normalAttachmentRef{};
	normalAttachmentRef.attachment = kAttachment_GBUFFER_VIEWNORMAL;
	normalAttachmentRef.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	pAoMapPass->AddInputReference(normalAttachmentRef);


	VkAttachmentReference positionAttachmentRef{};
	positionAttachmentRef.attachment = kAttachment_GBUFFER_VIEWPOS;
	positionAttachmentRef.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	
	pAoMapPass->AddInputReference(positionAttachmentRef);

	// Depth prepass depth buffer reference (read/write)
	VkAttachmentReference depthAttachmentReference{};
	depthAttachmentReference.attachment = kAttachment_DEPTH;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

	pAoMapPass->AddInputReference(depthAttachmentReference);

	VkAttachmentReference aoMapReference{};
	aoMapReference.attachment = kAttachment_AO_MAP;
	aoMapReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	pAoMapPass->AddReference(aoMapReference);	


	m_pRenderpass->AddSubpass(std::move(pAoMapPass));
}

void DDM3::SSAORenderer::SetupAoBlurPass()
{
	std::unique_ptr<Subpass> pBlurSubpass{std::make_unique<Subpass>()};

	VkAttachmentReference aoMapReference{};
	aoMapReference.attachment = kAttachment_AO_MAP;
	aoMapReference.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	pBlurSubpass->AddInputReference(aoMapReference);

	VkAttachmentReference aoBlurReference{};
	aoBlurReference.attachment = kAttachment_AO_BLURRED;
	aoBlurReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	pBlurSubpass->AddReference(aoBlurReference);


	m_pRenderpass->AddSubpass(std::move(pBlurSubpass));
}

void DDM3::SSAORenderer::SetupLightingPass()
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

	VkAttachmentReference aoAttachmentRef{};
	aoAttachmentRef.attachment = kAttachment_AO_BLURRED;
	aoAttachmentRef.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	pLightingPass->AddInputReference(aoAttachmentRef);

	// Final pass-back buffer render reference
	VkAttachmentReference backBufferRenderRef{};
	backBufferRenderRef.attachment = kAttachment_BACK;
	backBufferRenderRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	pLightingPass->AddReference(backBufferRenderRef);

	m_pRenderpass->AddSubpass(std::move(pLightingPass));
}

void DDM3::SSAORenderer::SetupImGuiPass()
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

void DDM3::SSAORenderer::SetupDependencies()
{
	VkSubpassDependency depthToGbufferDependency{};
	depthToGbufferDependency.srcSubpass = kSubpass_DEPTH;
	depthToGbufferDependency.dstSubpass = kSubpass_GBUFFER;
	depthToGbufferDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	depthToGbufferDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	depthToGbufferDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	depthToGbufferDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	depthToGbufferDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkSubpassDependency gBufferToAoGenDependency{};
	gBufferToAoGenDependency.srcSubpass = kSubpass_GBUFFER;
	gBufferToAoGenDependency.dstSubpass = kSubpass_AO_GEN;
	gBufferToAoGenDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	gBufferToAoGenDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	gBufferToAoGenDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	gBufferToAoGenDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	gBufferToAoGenDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkSubpassDependency aoGenToAoBlurDependency{};
	aoGenToAoBlurDependency.srcSubpass = kSubpass_AO_GEN;
	aoGenToAoBlurDependency.dstSubpass = kSubpass_AO_BLUR;
	aoGenToAoBlurDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	aoGenToAoBlurDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	aoGenToAoBlurDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	aoGenToAoBlurDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	aoGenToAoBlurDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkSubpassDependency aoBlurToLightingDependency{};
	aoBlurToLightingDependency.srcSubpass = kSubpass_AO_BLUR;
	aoBlurToLightingDependency.dstSubpass = kSubpass_LIGHTING;
	aoBlurToLightingDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	aoBlurToLightingDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	aoBlurToLightingDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	aoBlurToLightingDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	aoBlurToLightingDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkSubpassDependency lightingToImguiDependency{};
	lightingToImguiDependency.srcSubpass = kSubpass_LIGHTING;
	lightingToImguiDependency.dstSubpass = kSubpass_IMGUI;
	lightingToImguiDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	lightingToImguiDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	lightingToImguiDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	lightingToImguiDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	lightingToImguiDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


	m_pRenderpass->AddDependency(depthToGbufferDependency);
	m_pRenderpass->AddDependency(gBufferToAoGenDependency);
	m_pRenderpass->AddDependency(aoGenToAoBlurDependency);
	m_pRenderpass->AddDependency(aoBlurToLightingDependency);
	m_pRenderpass->AddDependency(lightingToImguiDependency);;
}

void DDM3::SSAORenderer::SetupDescriptorObjects()
{
	SetupDescriptorObjectsAoGen();

	SetupDescriptorObjectsAoBlur();

	SetupDescriptorObjectsLighting();
}

void DDM3::SSAORenderer::SetupDescriptorObjectsAoGen()
{
	m_pAoGenInputDescriptorObjects.clear();

	auto& attachments{ m_pRenderpass->GetAttachmentList() };

	// Normal texture
	auto descriptorObject{ std::make_unique<InputAttachmentDescriptorObject>() };

	descriptorObject->AddImageView(attachments[kAttachment_GBUFFER_VIEWNORMAL]->GetTexture(0)->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	m_pAoGenInputDescriptorObjects.push_back(std::move(descriptorObject));

	// Depth texture
	descriptorObject = std::make_unique<InputAttachmentDescriptorObject>();

	descriptorObject->AddImageView(attachments[kAttachment_DEPTH]->GetTexture(0)->imageView, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);

	m_pAoGenInputDescriptorObjects.push_back(std::move(descriptorObject));
}

void DDM3::SSAORenderer::SetupDescriptorObjectsAoBlur()
{
	m_pAoGenTextureDescriptorObject = std::make_unique<TextureDescriptorObject>();
	m_pAoGenTextureDescriptorObject->SetCleanupTextures(false);
}

void DDM3::SSAORenderer::SetupDescriptorObjectsLighting()
{
	m_pLightingInputDescriptorObjects.clear();

	auto& attachments{ m_pRenderpass->GetAttachmentList() };

	for (int i{ kAttachment_GBUFFER_ALBEDO }; i <= kAttachment_GBUFFER_POSITION; ++i)
	{
		auto descriptorObject{ std::make_unique<InputAttachmentDescriptorObject>() };

		descriptorObject->AddImageView(attachments[i]->GetTexture(0)->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		m_pLightingInputDescriptorObjects.push_back(std::move(descriptorObject));
	}

	auto descriptorObject{ std::make_unique<InputAttachmentDescriptorObject>() };

	descriptorObject->AddImageView(attachments[kAttachment_AO_MAP]->GetTexture(0)->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	m_pLightingInputDescriptorObjects.push_back(std::move(descriptorObject));
}

void DDM3::SSAORenderer::SetupPositionTexture()
{
	m_pPositionTextureDescriptorObject = std::make_unique<TextureDescriptorObject>();
	m_pPositionTextureDescriptorObject->SetCleanupTextures(false);
}

void DDM3::SSAORenderer::SetupNoiseTexture()
{
	m_pNoiseTextureDescriptorObject = std::make_unique<TextureDescriptorObject>();
	m_pNoiseTextureDescriptorObject->AddTexture("Resources/Images/NoiseTexture2.png");
}

void DDM3::SSAORenderer::SetupSamples()
{
	auto maxFrames = VulkanObject::GetInstance().GetMaxFrames();

	m_Samples = std::vector<std::vector<glm::vec4>>(maxFrames);

	for (auto& sampleList : m_Samples)
	{
		sampleList = std::vector<glm::vec4>(m_SampleCount);
	}

	m_pSamplesDescriptorObject = std::make_unique<UboDescriptorObject<glm::vec4>>(m_SampleCount);
}

void DDM3::SSAORenderer::SetNewSamples(int frame, int swapchainIndex)
{
	for (int i{}; i < m_Samples[frame].size(); ++i)
	{
		GetRandomVector(m_Samples[frame][i], i);
	}
	
	UpdateAoGenDescriptorSets(frame, swapchainIndex);
}


// Logic by Brian Will
// https://www.youtube.com/watch?v=7hxrPKoELpo
void DDM3::SSAORenderer::GetRandomVector(glm::vec4& vec, int index)
{
	vec.x = Utils::RandomFLoat(-1.0f, 1.0f);
	vec.y = Utils::RandomFLoat(-1.0f, 1.0f);
	vec.z = Utils::RandomFLoat(0.0f, 1.0f);
	vec.w = 0;

	vec = glm::normalize(vec);
	vec *= Utils::RandomFLoat(0.0f, 1.0f);

	// This logic will cluster the vectors near the origin
	float scale = static_cast<float>(index) / static_cast<float>(m_SampleCount);
	vec *= Utils::Lerp(0.1f, 1.0f, scale * scale);
}

void DDM3::SSAORenderer::SetupProjectionMatrix()
{
	m_pProjectionMatrixDescObject = std::make_unique<UboDescriptorObject<glm::mat4>>();
}

void DDM3::SSAORenderer::InitImgui()
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
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Subpass = kSubpass_IMGUI;

	// Create a single time command buffer
	auto commandBuffer{ VulkanObject::GetInstance().BeginSingleTimeCommands() };
	// Initialize ImGui
	m_pImGuiWrapper = std::make_unique<DDM3::ImGuiWrapper>(init_info,
		VulkanObject::GetInstance().GetDevice(), static_cast<uint32_t>(VulkanObject::GetInstance().GetMaxFrames()), m_pRenderpass->GetRenderpass(), commandBuffer);
	// End the single time command buffer
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);
}

void DDM3::SSAORenderer::CleanupImgui()
{
	m_pImGuiWrapper.reset();
}

void DDM3::SSAORenderer::RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex)
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
	
	// Depth prepass
	m_pRenderpass->BeginRenderPass(commandBuffer, m_pSwapchainWrapper->GetFrameBuffer(imageIndex, m_pRenderpass.get()), extent);

	SceneManager::GetInstance().RenderDepth();

	// G-buffer pass
	vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

	SceneManager::GetInstance().Render();

	SceneManager::GetInstance().RenderTransparancy();

	// AO Map pass
	vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pAoPipeline->GetPipeline());

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pAoPipeline->GetPipelineLayout(), 0, 1,
		&m_AoGenDescriptorSets[frame], 0, nullptr);

	VulkanObject::GetInstance().DrawQuad(commandBuffer);

	// AO Blur pass
	vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pAoBlurPipeline->GetPipeline());

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pAoBlurPipeline->GetPipelineLayout(), 0, 1,
		&m_AoBlurDescriptorSets[frame], 0, nullptr);

	VulkanObject::GetInstance().DrawQuad(commandBuffer);


	// Lighting pass
	vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pLightingPipeline->GetPipeline());

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pLightingPipeline->GetPipelineLayout(), 0, 1,
		&m_LightingDescriptorSets[frame], 0, nullptr);

	VulkanObject::GetInstance().DrawQuad(commandBuffer);


	// ImgGui pass
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

void DDM3::SSAORenderer::RecreateSwapChain()
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


	std::vector<RenderpassWrapper*> renderpasses{ m_pRenderpass.get()};

	// Recreate the swapchain
	m_pSwapchainWrapper->RecreateSwapChain(DDM3::VulkanObject::GetInstance().GetGPUObject(), DDM3::VulkanObject::GetInstance().GetSurface(),
		VulkanObject::GetInstance().GetImageManager(),
		commandBuffer, renderpasses);

	// End single time command
	VulkanObject::GetInstance().EndSingleTimeCommands(commandBuffer);

	ResetDescriptorSets();
}

void DDM3::SSAORenderer::ResetDescriptorSets()
{
	vkFreeDescriptorSets(VulkanObject::GetInstance().GetDevice(), m_LightingDescriptorPool, m_LightingDescriptorSets.size(), m_LightingDescriptorSets.data());

	vkFreeDescriptorSets(VulkanObject::GetInstance().GetDevice(), m_AoGenDescriptorPool, m_AoGenDescriptorSets.size(), m_AoGenDescriptorSets.data());

	vkFreeDescriptorSets(VulkanObject::GetInstance().GetDevice(), m_AoBlurDescriptorPool, m_AoBlurDescriptorSets.size(), m_AoBlurDescriptorSets.data());

	SetupDescriptorObjects();

	CreateDescriptorSets();
}

void DDM3::SSAORenderer::CreateDescriptorSetLayouts()
{
	CreateAoGenDescriptorSetLayout();

	CreateAoBlurDescriptorSetLayout();

	CreateLightingDescriptorSetLayout();
}

void DDM3::SSAORenderer::CreateAoGenDescriptorSetLayout()
{
	// Create vector of descriptorsetlayoutbindings the size of the sum of vertexUbos, fragmentUbos and textureamount;
	std::vector<VkDescriptorSetLayoutBinding> bindings{};

	// Add the descriptor layout bindings for each shader module
	for (int i{}; i < 2; ++i)
	{
		VkDescriptorSetLayoutBinding binding{};

		binding.binding = i;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		binding.descriptorCount = 1;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		binding.pImmutableSamplers = nullptr;

		bindings.push_back(binding);
	}

	VkDescriptorSetLayoutBinding posTextureBinding{};

	posTextureBinding.binding = 2;
	posTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	posTextureBinding.descriptorCount = 1;
	posTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	posTextureBinding.pImmutableSamplers = nullptr;

	bindings.push_back(posTextureBinding);

	VkDescriptorSetLayoutBinding noiseTextureBinding{};

	noiseTextureBinding.binding = 3;
	noiseTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	noiseTextureBinding.descriptorCount = 1;
	noiseTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	noiseTextureBinding.pImmutableSamplers = nullptr;

	bindings.push_back(noiseTextureBinding);

	VkDescriptorSetLayoutBinding samplesBinding{};

	samplesBinding.binding = 4;
	samplesBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	samplesBinding.descriptorCount = 1;
	samplesBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplesBinding.pImmutableSamplers = nullptr;

	bindings.push_back(samplesBinding);


	VkDescriptorSetLayoutBinding projectionMatrixBinding{};

	projectionMatrixBinding.binding = 5;
	projectionMatrixBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	projectionMatrixBinding.descriptorCount = 1;
	projectionMatrixBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	projectionMatrixBinding.pImmutableSamplers = nullptr;

	bindings.push_back(projectionMatrixBinding);

	// Create layout info
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	// Set type to descriptor set layout create info
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	// Set bindingcount to the amount of bindings
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	// Set bindings to the data of bindings vector
	layoutInfo.pBindings = bindings.data();

	// Create descriptorset layout
	if (vkCreateDescriptorSetLayout(VulkanObject::GetInstance().GetDevice(), &layoutInfo, nullptr, &m_AoGenDescriptorSetLayout) != VK_SUCCESS)
	{
		// If not successfull, throw runtime error
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void DDM3::SSAORenderer::CreateAoBlurDescriptorSetLayout()
{
	// Create vector of descriptorsetlayoutbindings the size of the sum of vertexUbos, fragmentUbos and textureamount;
	std::vector<VkDescriptorSetLayoutBinding> bindings{};

	VkDescriptorSetLayoutBinding aoGenTextureBinding{};

	aoGenTextureBinding.binding = 0;
	aoGenTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	aoGenTextureBinding.descriptorCount = 1;
	aoGenTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	aoGenTextureBinding.pImmutableSamplers = nullptr;

	bindings.push_back(aoGenTextureBinding);

	// Create layout info
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	// Set type to descriptor set layout create info
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	// Set bindingcount to the amount of bindings
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	// Set bindings to the data of bindings vector
	layoutInfo.pBindings = bindings.data();

	// Create descriptorset layout
	if (vkCreateDescriptorSetLayout(VulkanObject::GetInstance().GetDevice(), &layoutInfo, nullptr, &m_AoBlurDescriptorSetLayout) != VK_SUCCESS)
	{
		// If not successfull, throw runtime error
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void DDM3::SSAORenderer::CreateLightingDescriptorSetLayout()
{
	// Create vector of descriptorsetlayoutbindings the size of the sum of vertexUbos, fragmentUbos and textureamount;
	std::vector<VkDescriptorSetLayoutBinding> bindings{};

	// Add the descriptor layout bindings for each shader module
	for (int i{}; i < 4; ++i)
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
	if (vkCreateDescriptorSetLayout(VulkanObject::GetInstance().GetDevice(), &layoutInfo, nullptr, &m_LightingDescriptorSetLayout) != VK_SUCCESS)
	{
		// If not successfull, throw runtime error
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void DDM3::SSAORenderer::CreateDescriptorPools()
{
	CreateAoGenDescriptorPool();

	CreateAoBlurDescriptorPool();

	CreateLightingDescriptorPool();
}

void DDM3::SSAORenderer::CreateAoGenDescriptorPool()
{
	// Get a reference to the vulkan object
	auto& vulkanObject{ VulkanObject::GetInstance() };

	// Get the amount of frames in flight
	auto frames{ vulkanObject.GetMaxFrames() };

	// Create a vector of pool sizes
	std::vector<VkDescriptorPoolSize> poolSizes{};

	// Loop trough all the descriptor types
	for (int i{}; i < 2; ++i)
	{
		VkDescriptorPoolSize descriptorPoolSize{};
		// Set the type of the poolsize
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		// Calculate the amount of descriptors needed from this type
		descriptorPoolSize.descriptorCount = frames;

		// Add the descriptor poolsize to the vector
		poolSizes.push_back(descriptorPoolSize);
	}

	VkDescriptorPoolSize posTextureDescriptorpoolSize{};
	posTextureDescriptorpoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	posTextureDescriptorpoolSize.descriptorCount = frames;

	poolSizes.push_back(posTextureDescriptorpoolSize);

	VkDescriptorPoolSize noiseTextureDescriptorpoolSize{};
	noiseTextureDescriptorpoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	noiseTextureDescriptorpoolSize.descriptorCount = frames;

	poolSizes.push_back(noiseTextureDescriptorpoolSize);

	VkDescriptorPoolSize samplesDescriptorPoolSize{};
	samplesDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	samplesDescriptorPoolSize.descriptorCount = frames;

	poolSizes.push_back(samplesDescriptorPoolSize);

	VkDescriptorPoolSize projectionMatrixPoolSize{};
	projectionMatrixPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	projectionMatrixPoolSize.descriptorCount = frames;

	poolSizes.push_back(projectionMatrixPoolSize);


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
	if (vkCreateDescriptorPool(vulkanObject.GetDevice(), &poolInfo, nullptr, &m_AoGenDescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void DDM3::SSAORenderer::CreateAoBlurDescriptorPool()
{
	// Get a reference to the vulkan object
	auto& vulkanObject{ VulkanObject::GetInstance() };

	// Get the amount of frames in flight
	auto frames{ vulkanObject.GetMaxFrames() };

	// Create a vector of pool sizes
	std::vector<VkDescriptorPoolSize> poolSizes{};
	

	VkDescriptorPoolSize aogGenTextureDescriptorpoolSize{};
	aogGenTextureDescriptorpoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	aogGenTextureDescriptorpoolSize.descriptorCount = frames;

	poolSizes.push_back(aogGenTextureDescriptorpoolSize);


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
	if (vkCreateDescriptorPool(vulkanObject.GetDevice(), &poolInfo, nullptr, &m_AoBlurDescriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void DDM3::SSAORenderer::CreateLightingDescriptorPool()
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
	if (vkCreateDescriptorPool(vulkanObject.GetDevice(), &poolInfo, nullptr, &m_LightingDescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void DDM3::SSAORenderer::CreateDescriptorSets()
{
	CreateAoGenDescriptorSets();

	CreateAoBlurDescriptorSets();

	CreateLightingDescriptorSets();
}

void DDM3::SSAORenderer::CreateAoGenDescriptorSets()
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
	allocInfo.descriptorPool = m_AoGenDescriptorPool;
	// Give the amount of descriptorsets to be allocated
	allocInfo.descriptorSetCount = static_cast<uint32_t>(maxFrames);
	// Create vector of descriptorsets the size of maxFrames and fill with layout
	std::vector<VkDescriptorSetLayout> layouts(maxFrames, m_AoGenDescriptorSetLayout);
	allocInfo.pSetLayouts = layouts.data();

	// Resize descriptorsets to right amount
	m_AoGenDescriptorSets.resize(maxFrames);

	// Allocate descriptorsets, if not succeeded, throw runtime error
	if (vkAllocateDescriptorSets(renderer.GetDevice(), &allocInfo, m_AoGenDescriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (int i{}; i < renderer.GetMaxFrames(); ++i)
	{
		UpdateAoGenDescriptorSets(i, 0);
	}
}

void DDM3::SSAORenderer::CreateAoBlurDescriptorSets()
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
	allocInfo.descriptorPool = m_AoBlurDescriptorPool;
	// Give the amount of descriptorsets to be allocated
	allocInfo.descriptorSetCount = static_cast<uint32_t>(maxFrames);
	// Create vector of descriptorsets the size of maxFrames and fill with layout
	std::vector<VkDescriptorSetLayout> layouts(maxFrames, m_AoBlurDescriptorSetLayout);
	allocInfo.pSetLayouts = layouts.data();

	// Resize descriptorsets to right amount
	m_AoBlurDescriptorSets.resize(maxFrames);

	// Allocate descriptorsets, if not succeeded, throw runtime error
	if (vkAllocateDescriptorSets(renderer.GetDevice(), &allocInfo, m_AoBlurDescriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (int i{}; i < renderer.GetMaxFrames(); ++i)
	{
		UpdateAoBlurDescriptorSets(i, 0);
	}
}

void DDM3::SSAORenderer::CreateLightingDescriptorSets()
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
	allocInfo.descriptorPool = m_LightingDescriptorPool;
	// Give the amount of descriptorsets to be allocated
	allocInfo.descriptorSetCount = static_cast<uint32_t>(maxFrames);
	// Create vector of descriptorsets the size of maxFrames and fill with layout
	std::vector<VkDescriptorSetLayout> layouts(maxFrames, m_LightingDescriptorSetLayout);
	allocInfo.pSetLayouts = layouts.data();

	// Resize descriptorsets to right amount
	m_LightingDescriptorSets.resize(maxFrames);

	// Allocate descriptorsets, if not succeeded, throw runtime error
	if (vkAllocateDescriptorSets(renderer.GetDevice(), &allocInfo, m_LightingDescriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (int i{}; i < renderer.GetMaxFrames(); ++i)
	{
		UpdateLightingDescriptorSets(i);
	}
}

void DDM3::SSAORenderer::UpdateDescriptorSets(int frame, int swapchainIndex)
{
	UpdateAoGenDescriptorSets(frame, swapchainIndex);

	UpdateAoBlurDescriptorSets(frame, swapchainIndex);

	UpdateLightingDescriptorSets(frame);
}

void DDM3::SSAORenderer::UpdateAoGenDescriptorSets(int frame, int swapchainIndex)
{
		// Create a vector of descriptor writes
		std::vector<VkWriteDescriptorSet> descriptorWrites{};

		// Initialize current binding with 0
		int binding{};

		// Loop trough all descriptor objects and add the descriptor writes
		for (auto& descriptorObject : m_pAoGenInputDescriptorObjects)
		{
			descriptorObject->AddDescriptorWrite(m_AoGenDescriptorSets[frame], descriptorWrites, binding, 1, frame);
		}

		m_pPositionTextureDescriptorObject->Clear();

		auto& posTexture = m_pRenderpass->GetAttachmentList()[kAttachment_GBUFFER_VIEWPOS]->GetTextureRef(swapchainIndex);
		m_pPositionTextureDescriptorObject->AddTextures(posTexture);

		m_pPositionTextureDescriptorObject->AddDescriptorWrite(m_AoGenDescriptorSets[frame], descriptorWrites, binding, 1, frame);

		m_pNoiseTextureDescriptorObject->AddDescriptorWrite(m_AoGenDescriptorSets[frame], descriptorWrites, binding, 1, frame);

		m_pSamplesDescriptorObject->UpdateUboBuffer(m_Samples[frame].data(), frame);

		m_pSamplesDescriptorObject->AddDescriptorWrite(m_AoGenDescriptorSets[frame], descriptorWrites, binding, 1, frame);

		auto camera = SceneManager::GetInstance().GetCamera();

		if (camera != nullptr)
		{
			m_pProjectionMatrixDescObject->UpdateUboBuffer(camera->GetProjectionMatrixPointer(), frame);
		}

		m_pProjectionMatrixDescObject->AddDescriptorWrite(m_AoGenDescriptorSets[frame], descriptorWrites, binding, 1, frame);

		//vkDeviceWaitIdle(VulkanRenderer::GetInstance().GetDevice());
		// Update descriptorsets
		vkUpdateDescriptorSets(VulkanObject::GetInstance().GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void DDM3::SSAORenderer::UpdateAoBlurDescriptorSets(int frame, int swapchainIndex)
{
	// Create a vector of descriptor writes
	std::vector<VkWriteDescriptorSet> descriptorWrites{};

	// Initialize current binding with 0
	int binding{};


	auto& aoGenTexture = m_pRenderpass->GetAttachmentList()[kAttachment_AO_MAP]->GetTextureRef(swapchainIndex);
	m_pAoGenTextureDescriptorObject->AddTextures(aoGenTexture);

	m_pAoGenTextureDescriptorObject->AddDescriptorWrite(m_AoBlurDescriptorSets[frame], descriptorWrites, binding, 1, frame);


	//vkDeviceWaitIdle(VulkanRenderer::GetInstance().GetDevice());
	// Update descriptorsets
	vkUpdateDescriptorSets(VulkanObject::GetInstance().GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

}

void DDM3::SSAORenderer::UpdateLightingDescriptorSets(int frame)
{
		// Create a vector of descriptor writes
		std::vector<VkWriteDescriptorSet> descriptorWrites{};

		// Initialize current binding with 0
		int binding{};

		// Loop trough all descriptor objects and add the descriptor writes
		for (auto& descriptorObject : m_pLightingInputDescriptorObjects)
		{
			descriptorObject->AddDescriptorWrite(m_LightingDescriptorSets[frame], descriptorWrites, binding, 1, frame);
		}


		//vkDeviceWaitIdle(VulkanRenderer::GetInstance().GetDevice());
		// Update descriptorsets
		vkUpdateDescriptorSets(VulkanObject::GetInstance().GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}
