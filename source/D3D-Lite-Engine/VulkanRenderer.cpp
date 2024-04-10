#include "VulkanRenderer.h"
#include "D3DEngine.h"
#include "Utils.h"
#include <set>
#include <algorithm>

#include "SceneManager.h"
#include "DescriptorPoolManager.h"

#include "ModelComponent.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "SurfaceWrapper.h"
#include "CommandpoolManager.h"
#include "BufferManager.h"
#include "ImageManager.h"
#include "SwapchainWrapper.h"

#include "STBIncludes.h"
#include "ImGuiIncludes.h"
#include "Window.h"

#include "InstanceWrapper.h"
#include "GPUObject.h"
#include "ImGuiWrapper.h"
#include "SyncObjectManager.h"

#include "VulkanUtils.h"

D3D::VulkanRenderer::VulkanRenderer()
{
	m_GlobalLight.direction = glm::normalize(glm::vec3{ -.577, -.577f, 0 });
	m_GlobalLight.color = glm::vec3{ 1.f, 1.f, 1.f };
	m_GlobalLight.intensity = 1.f;

	InitVulkan();

	InitImGui();
}

D3D::VulkanRenderer::~VulkanRenderer()
{
	m_pGpuObject->WaitIdle();

	CleanupImGui();

	CleanupVulkan();
}

void D3D::VulkanRenderer::CleanupVulkan()
{
	auto device{ m_pGpuObject->GetDevice() };

	m_pSwapchainWrapper->Cleanup(device);
	
	m_pImageManager->Cleanup(device);

	for (size_t i{}; i < m_MaxFramesInFlight; i++)
	{
		vkDestroyBuffer(device, m_LightBuffers[i], nullptr);
		vkFreeMemory(device, m_LightMemory[i], nullptr);
	}

	m_pDescriptorPoolManager->Cleanup(device);

	for (auto& pair : m_DescriptorSetLayouts)
	{
		vkDestroyDescriptorSetLayout(device, pair.second, nullptr);
	}

	for (auto& pipeline : m_GraphicPipelines)
	{
		vkDestroyPipeline(device, pipeline.second.pipeline, nullptr);
		vkDestroyPipelineLayout(device, pipeline.second.pipelineLayout, nullptr);
	}

	vkDestroyRenderPass(device, m_RenderPass, nullptr);

	m_pSyncObjectManager->Cleanup(device);

	m_pCommandPoolManager->Cleanup(device);

	m_pGpuObject->CleanUp();

	m_pSurfaceWrapper->Cleanup(m_pInstanceWrapper->GetInstance());
}

void D3D::VulkanRenderer::CleanupImGui()
{
	m_pImGuiWrapper->Cleanup(m_pGpuObject->GetDevice());
}

void D3D::VulkanRenderer::InitVulkan()
{
	m_pBufferManager = std::make_unique<BufferManager>();

	m_pInstanceWrapper = std::make_unique<InstanceWrapper>();

	m_pSurfaceWrapper = std::make_unique<SurfaceWrapper>(m_pInstanceWrapper->GetInstance());

	m_pGpuObject = std::make_unique<GPUObject>(m_pInstanceWrapper.get(), m_pSurfaceWrapper->GetSurface());

	// Get the max amount of samples per pixel
	m_MsaaSamples = VulkanUtils::GetMaxUsableSampleCount(m_pGpuObject->GetPhysicalDevice());

	m_pCommandPoolManager = std::make_unique<CommandpoolManager>(m_pGpuObject.get(), m_pSurfaceWrapper->GetSurface(),
		static_cast<uint32_t>(m_MaxFramesInFlight));

	// Initialize the image manager
	m_pImageManager = std::make_unique<ImageManager>(m_pGpuObject.get(), m_pBufferManager.get(), m_pCommandPoolManager.get());

	m_pSwapchainWrapper = std::make_unique<SwapchainWrapper>(m_pGpuObject.get(), m_pSurfaceWrapper->GetSurface(),
		m_pImageManager.get(), m_MsaaSamples);

	CreateRenderPass();

	// Create a single time command buffer
	auto commandBuffer{ BeginSingleTimeCommands() };
	// Initialize swapchain
	m_pSwapchainWrapper->SetupImageViews(m_pGpuObject.get(), m_pImageManager.get(), commandBuffer, m_RenderPass);
	// End the single time command buffer
	EndSingleTimeCommands(commandBuffer);


	CreateLightBuffer();

	AddGraphicsPipeline(m_DefaultPipelineName, m_DefaultVertName, m_DefaultFragName, 1, 1, 0);


	m_pDescriptorPoolManager = std::make_unique<DescriptorPoolManager>();

	// Initialize the sync objects
	m_pSyncObjectManager = std::make_unique<SyncObjectManager>(m_pGpuObject->GetDevice(),
		static_cast<uint32_t>(m_MaxFramesInFlight));
}

void D3D::VulkanRenderer::InitImGui()
{
	// Create ImGui vulkan init info
	ImGui_ImplVulkan_InitInfo init_info = {};
	// Give the vulkan instance
	init_info.Instance = m_pInstanceWrapper->GetInstance();
	// Give the physical device
	init_info.PhysicalDevice = m_pGpuObject->GetPhysicalDevice();
	// Give the logical device
	init_info.Device = m_pGpuObject->GetDevice();
	// Give the index of the graphics queue family
	init_info.QueueFamily = m_pGpuObject->GetQueueObject().graphicsQueueIndex;
	// Give the graphics queue
	init_info.Queue = m_pGpuObject->GetQueueObject().graphicsQueue;
	// Set pipeline cache to null handle
	init_info.PipelineCache = VK_NULL_HANDLE;
	// Set Allocator to null handle
	init_info.Allocator = VK_NULL_HANDLE;
	// Set min image count to the minimum image count of the swapchain
	init_info.MinImageCount = m_pSwapchainWrapper->GetMinImageCount();
	// Set the image count to the max amount of frames in flight
	init_info.ImageCount = static_cast<uint32_t>(m_MaxFramesInFlight);
	// Give functoin for error handling
	init_info.CheckVkResultFn = [](VkResult /*err*/) { /* error handling */ };
	// Give the max amount of samples per mixel
	init_info.MSAASamples = m_MsaaSamples;

	// Create a single time command buffer
	auto commandBuffer{ BeginSingleTimeCommands() };
	// Initialize ImGui
	m_pImGuiWrapper = std::make_unique<D3D::ImGuiWrapper>(init_info, m_RenderPass, commandBuffer, m_pGpuObject->GetDevice(), static_cast<uint32_t>(m_MaxFramesInFlight));
	// End the single time command buffer
	EndSingleTimeCommands(commandBuffer);
}

void D3D::VulkanRenderer::AddGraphicsPipeline(const std::string& pipelineName, const std::string& vertShaderName, const std::string& fragShaderName, int vertexUbos, int fragmentUbos, int textureAmount)
{
	auto device{ m_pGpuObject->GetDevice() };

	if (m_GraphicPipelines.contains(pipelineName))
	{
		vkDestroyPipeline(device, m_GraphicPipelines[pipelineName].pipeline, nullptr);
		vkDestroyPipelineLayout(device, m_GraphicPipelines[pipelineName].pipelineLayout, nullptr);
	}

	auto vertShaderCode = Utils::readFile(vertShaderName);
	auto fragShaderCode = Utils::readFile(fragShaderName);

	VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescription = Vertex::getAttributeDescription();

	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	//--Dynamic viewport--

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;


	//--Static viewport--

	/*VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_SwapChainExtent.width);
	viewport.height = static_cast<float>(m_SwapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = m_SwapChainExtent;*/

	/*VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;*/

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; //Optional
	rasterizer.depthBiasClamp = 0.0f; //Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; //Optional

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_TRUE;
	multisampling.rasterizationSamples = m_MsaaSamples;
	multisampling.minSampleShading = .2f; //Optional
	multisampling.pSampleMask = nullptr; //Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; //Optional
	multisampling.alphaToOneEnable = VK_FALSE; //Optional

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; //Optional
	depthStencil.maxDepthBounds = 1.0f; //Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; //Optional
	depthStencil.back = {}; //Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; //Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; //Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; //Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; //Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; //Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; //Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; //Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; //Optional
	colorBlending.blendConstants[1] = 0.0f; //Optional
	colorBlending.blendConstants[2] = 0.0f; //Optional
	colorBlending.blendConstants[3] = 0.0f; //Otional

	/*VkPushConstantRange pushConstantRange = {};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(LightObject);*/

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = GetDescriptorSetLayout(vertexUbos, fragmentUbos, textureAmount);
	//pipelineLayoutInfo.pushConstantRangeCount = 1; // Number of push constant ranges used by the pipeline
	//pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Array of push constant ranges used by the pipeline

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_GraphicPipelines[pipelineName].pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.layout = m_GraphicPipelines[pipelineName].pipelineLayout;
	pipelineInfo.renderPass = m_RenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;


	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicPipelines[pipelineName].pipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

VkDevice D3D::VulkanRenderer::GetDevice()
{
	return m_pGpuObject->GetDevice();
}

VkImageView& D3D::VulkanRenderer::GetDefaultImageView()
{
	return m_pImageManager->GetDefaultImageView();
}

VkSampler& D3D::VulkanRenderer::GetSampler()
{
	return m_pImageManager->GetSampler();
}

VkDescriptorSetLayout* D3D::VulkanRenderer::GetDescriptorSetLayout(int vertexUbos, int fragmentUbos, int textureAmount)
{
	std::tuple<int, int, int> tuple{ vertexUbos, fragmentUbos, textureAmount };

	if (!m_DescriptorSetLayouts.contains(tuple))
		CreateDescriptorLayout(vertexUbos, fragmentUbos, textureAmount);

	return &m_DescriptorSetLayouts[tuple];
}

void D3D::VulkanRenderer::Render()
{
	auto device{ m_pGpuObject->GetDevice() };
	auto queueObject{ m_pGpuObject->GetQueueObject() };

	vkWaitForFences(device, 1, &m_pSyncObjectManager->GetInFlightFence(m_CurrentFrame), VK_TRUE, UINT64_MAX);

	uint32_t imageIndex{};
	VkResult result = vkAcquireNextImageKHR(device, m_pSwapchainWrapper->GetSwapchain(),
		UINT64_MAX, m_pSyncObjectManager->GetImageAvailableSemaphore(m_CurrentFrame), VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}



	vkResetFences(device, 1, &m_pSyncObjectManager->GetInFlightFence(m_CurrentFrame));

	auto commandBuffer{ GetCurrentCommandBuffer() };
	vkResetCommandBuffer(commandBuffer, 0);
	RecordCommandBuffer(commandBuffer, imageIndex);


	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_pSyncObjectManager->GetImageAvailableSemaphore(m_CurrentFrame)};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &GetCurrentCommandBuffer();

	VkSemaphore signalSemaphores[] = { m_pSyncObjectManager->GetRenderFinishedSemaphore(m_CurrentFrame) };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(queueObject.graphicsQueue, 1, &submitInfo, m_pSyncObjectManager->GetInFlightFence(m_CurrentFrame)) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_pSwapchainWrapper->GetSwapchain()};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(queueObject.presentQueue, &presentInfo);


	auto& window{ Window::GetInstance().GetWindowStruct()};

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.FrameBufferResized)
	{
		RecreateSwapChain();
		window.FrameBufferResized = false;
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	++m_CurrentFrame %= m_MaxFramesInFlight;
}

void D3D::VulkanRenderer::RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex)
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
	renderPassInfo.renderPass = m_RenderPass;
	renderPassInfo.framebuffer = m_pSwapchainWrapper->GetFrameBuffer(imageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_pSwapchainWrapper->GetExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

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

	UpdateLightBuffer(m_CurrentFrame);

	SceneManager::GetInstance().Render();

	// Render the ImGui
	m_pImGuiWrapper->Render(commandBuffer);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void D3D::VulkanRenderer::Render(D3D::ModelComponent* pModel, VkCommandBuffer& commandBuffer, const VkDescriptorSet* descriptorSet, const PipelinePair& pipeline)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);

	VkBuffer vertexBuffers[] = { pModel->GetVertexBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer, pModel->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, descriptorSet, 0, nullptr);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(pModel->GetIndexAmount()), 1, 0, 0, 0);
}

D3D::PipelinePair& D3D::VulkanRenderer::GetPipeline(const std::string& name)
{
	if (m_GraphicPipelines.contains(name))
	{
		return m_GraphicPipelines[name];
	}
	else
	{
		return m_GraphicPipelines[m_DefaultPipelineName];
	}
}

VkCommandBuffer& D3D::VulkanRenderer::GetCurrentCommandBuffer()
{
	return m_pCommandPoolManager->GetCommandBuffer(m_CurrentFrame);
}

D3D::DescriptorPoolManager* D3D::VulkanRenderer::GetDescriptorPoolManager() const
{
	return m_pDescriptorPoolManager.get();
}

void D3D::VulkanRenderer::RecreateSwapChain()
{
	// Get a reference to the window struct
	auto& windowStruct{ Window::GetInstance().GetWindowStruct() };

	//Get the width and the height of the window
	glfwGetFramebufferSize(windowStruct.pWindow, &windowStruct.Width, &windowStruct.Height);

	// While width and height are 0, wait before continuing
	while (windowStruct.Width == 0 || windowStruct.Height == 0)
	{
		glfwGetFramebufferSize(windowStruct.pWindow, &windowStruct.Width, &windowStruct.Height);
		glfwWaitEvents();
	}

	// Wait until the device is idle
	m_pGpuObject->WaitIdle();

	// Create a single time command
	auto commandBuffer{ BeginSingleTimeCommands() };

	// Recreate the swapchain
	m_pSwapchainWrapper->RecreateSwapChain(m_pGpuObject.get(), m_pSurfaceWrapper->GetSurface(), m_pImageManager.get(),
		commandBuffer, m_RenderPass);

	// End single time command
	EndSingleTimeCommands(commandBuffer);
}

void D3D::VulkanRenderer::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_pSwapchainWrapper->GetFormat();
	colorAttachment.samples = m_MsaaSamples;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = VulkanUtils::FindDepthFormat(m_pGpuObject->GetPhysicalDevice());
	depthAttachment.samples = m_MsaaSamples;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


	VkAttachmentDescription colorAttachmentResolve{};
	colorAttachmentResolve.format = m_pSwapchainWrapper->GetFormat();
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentResolveRef{};
	colorAttachmentResolveRef.attachment = 2;
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	subpass.pResolveAttachments = &colorAttachmentResolveRef;


	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


	std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;


	if (vkCreateRenderPass(m_pGpuObject->GetDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

void D3D::VulkanRenderer::CreateDescriptorLayout(int vertexUbos, int fragmentUbos, int textureAmount)
{
	std::vector<VkDescriptorSetLayoutBinding> bindings(vertexUbos + fragmentUbos + textureAmount);

	for (int i{}; i < vertexUbos; ++i)
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = i;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		bindings[i] = uboLayoutBinding;
	}

	for (int i{ vertexUbos }; i < fragmentUbos + vertexUbos; ++i)
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = i;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		bindings[i] = uboLayoutBinding;
	}


	for (int i{ vertexUbos + fragmentUbos }; i < vertexUbos + fragmentUbos + textureAmount; ++i)
	{
		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = i;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		bindings[i] = samplerLayoutBinding;
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	std::tuple<int, int, int> tuple{ vertexUbos, fragmentUbos, textureAmount };

	m_DescriptorSetLayouts[tuple] = VK_NULL_HANDLE;

	if (vkCreateDescriptorSetLayout(m_pGpuObject->GetDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayouts[tuple]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

VkShaderModule D3D::VulkanRenderer::CreateShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_pGpuObject->GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

void D3D::VulkanRenderer::CreateLightBuffer()
{
	VkDeviceSize bufferSize = sizeof(LightObject);

	m_LightBuffers.resize(m_MaxFramesInFlight);
	m_LightMemory.resize(m_MaxFramesInFlight);
	m_LightMapped.resize(m_MaxFramesInFlight);

	m_LightChanged.resize(m_MaxFramesInFlight);
	std::fill(m_LightChanged.begin(), m_LightChanged.end(), true);



	for (size_t i = 0; i < m_MaxFramesInFlight; ++i)
	{
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_LightBuffers[i], m_LightMemory[i]);

		vkMapMemory(m_pGpuObject->GetDevice(), m_LightMemory[i], 0, bufferSize, 0, &m_LightMapped[i]);

		UpdateLightBuffer(static_cast<int>(i));
	}
}

void D3D::VulkanRenderer::UpdateLightBuffer(int frame)
{
	//if (!m_LightChanged[frame])
	//	return;

	m_LightChanged[frame] = false;

	memcpy(m_LightMapped[frame], &m_GlobalLight, sizeof(m_GlobalLight));
}

void D3D::VulkanRenderer::CreateTexture(Texture& texture, const std::string& textureName)
{
	// Create the image trough the image manager
	m_pImageManager->CreateTextureImage(m_pGpuObject.get(), m_pBufferManager.get(), texture, textureName, m_pCommandPoolManager.get());
	// Create the image view
	texture.imageView = m_pImageManager->CreateImageView(m_pGpuObject->GetDevice(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, texture.mipLevels);
}

void D3D::VulkanRenderer::CreateCubeTexture(Texture& cubeTexture, const std::initializer_list<const std::string>& textureNames)
{
	// Create a cube texture trough image manager
	m_pImageManager->CreateCubeTexture(m_pGpuObject.get(), m_pBufferManager.get(), cubeTexture, textureNames, m_pCommandPoolManager.get());
}
VkCommandBuffer D3D::VulkanRenderer::BeginSingleTimeCommands()
{
	return m_pCommandPoolManager->BeginSingleTimeCommands(m_pGpuObject->GetDevice());
}

void D3D::VulkanRenderer::EndSingleTimeCommands(VkCommandBuffer comandBuffer)
{
	m_pCommandPoolManager->EndSingleTimeCommands(m_pGpuObject.get(), comandBuffer);
}

void D3D::VulkanRenderer::UpdateUniformBuffer(UniformBufferObject& buffer)
{
	auto pCamera{SceneManager::GetInstance().GetCamera()};

	if (pCamera == nullptr)
		return;
	
	auto pCameraTransform{ pCamera->GetTransform()};

	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), pCameraTransform->GetWorldPosition());

	glm::quat quaternion = glm::quat(pCameraTransform->GetWorldRotation());
	glm::mat4 rotationMatrix = glm::mat4_cast(quaternion);

	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), pCameraTransform->GetWorldScale());

	buffer.view = translationMatrix * rotationMatrix * scalingMatrix;

	auto extent{ m_pSwapchainWrapper->GetExtent() };

	buffer.proj = glm::perspective(pCamera->GetFovAngle(), extent.width / static_cast<float>(extent.height), 0.1f, 10.0f);
	buffer.proj[1][1] *= -1;
}

bool D3D::VulkanRenderer::HasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void D3D::VulkanRenderer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	// Create the buffer trough vulkan utils
	m_pBufferManager->CreateBuffer(m_pGpuObject.get(), size, usage, properties, buffer, bufferMemory);
}

void D3D::VulkanRenderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	// Copy a buffer trough the bufferManager
	m_pBufferManager->CopyBuffer(m_pGpuObject.get(), m_pCommandPoolManager.get(), srcBuffer, dstBuffer, size);
}

void D3D::VulkanRenderer::CreateVertexBuffer(std::vector<D3D::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory)
{
	// Create a vertex buffer trough the buffer manager
	m_pBufferManager->CreateVertexBuffer(m_pGpuObject.get(), m_pCommandPoolManager.get(), vertices, vertexBuffer, vertexBufferMemory);
}

void D3D::VulkanRenderer::CreateIndexBuffer(std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory)
{
	// Create an index buffer trough the buffer manager
	m_pBufferManager->CreateIndexBuffer(m_pGpuObject.get(), m_pCommandPoolManager.get(), indices, indexBuffer, indexBufferMemory);
}