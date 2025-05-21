// VulkanRenderer.cpp

// Header include
#include "VulkanRenderer.h"

// File includes
#include "Engine/DDM3Engine.h"
#include "Engine/Window.h"
#include "Managers/SceneManager.h"

#include "Includes/STBIncludes.h"
#include "Includes/ImGuiIncludes.h"
#include "Includes/GLFWIncludes.h"

#include "VulkanUtils.h"
#include "Utils/Utils.h"

#include "Vulkan/VulkanWrappers/SurfaceWrapper.h"
#include "Vulkan/VulkanManagers/CommandpoolManager.h"
#include "Vulkan/VulkanManagers/BufferManager.h"
#include "Vulkan/VulkanManagers/ImageManager.h"
#include "Vulkan/VulkanWrappers/SwapchainWrapper.h"
#include "Vulkan/VulkanWrappers/RenderpassWrapper.h"
#include "Vulkan/VulkanManagers/PipelineManager.h"
#include "Vulkan/VulkanWrappers/PipelineWrapper.h"
#include "VulkanWrappers/InstanceWrapper.h"
#include "VulkanWrappers/GPUObject.h"
#include "VulkanWrappers/ImGuiWrapper.h"
#include "VulkanManagers/SyncObjectManager.h"

#include "Components/MeshRenderComponent.h"
#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Components/DirectionalLightComponent.h"

// Standard library includes
#include <set>
#include <algorithm>

DDM3::VulkanRenderer::VulkanRenderer()
{
	InitVulkan();

	InitImGui();
}

DDM3::VulkanRenderer::~VulkanRenderer()
{
	m_pGpuObject->WaitIdle();

	CleanupImGui();

	CleanupVulkan();
}

void DDM3::VulkanRenderer::CleanupVulkan()
{
	auto device{ m_pGpuObject->GetDevice() };

	m_pSwapchainWrapper->Cleanup(device);
	
	m_pImageManager->Cleanup(device);

	m_pPipelineManager->Cleanup(device);


	m_pRenderpassWrapper->cleanup(device);

	m_pSyncObjectManager->Cleanup(device);

	m_pCommandPoolManager->Cleanup(device);

	m_pGpuObject->CleanUp();

	m_pSurfaceWrapper->Cleanup(m_pInstanceWrapper->GetInstance());
}

void DDM3::VulkanRenderer::CleanupImGui()
{
	m_pImGuiWrapper->Cleanup(m_pGpuObject->GetDevice());
}

void DDM3::VulkanRenderer::InitVulkan()
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

	m_pRenderpassWrapper = std::make_unique<RenderpassWrapper>(GetDevice(), m_pSwapchainWrapper->GetFormat(),
		VulkanUtils::FindDepthFormat(m_pGpuObject->GetPhysicalDevice()), m_MsaaSamples);

	// Create a single time command buffer
	auto commandBuffer{ BeginSingleTimeCommands() };
	// Initialize swapchain
	m_pSwapchainWrapper->SetupImageViews(m_pGpuObject.get(), m_pImageManager.get(), commandBuffer,
		m_pRenderpassWrapper->GetRenderpass());
	// End the single time command buffer
	EndSingleTimeCommands(commandBuffer);

	m_pPipelineManager = std::make_unique<PipelineManager>();

	// Initialize the sync objects
	m_pSyncObjectManager = std::make_unique<SyncObjectManager>(m_pGpuObject->GetDevice(),
		static_cast<uint32_t>(m_MaxFramesInFlight));
}

void DDM3::VulkanRenderer::InitImGui()
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
	m_pImGuiWrapper = std::make_unique<DDM3::ImGuiWrapper>(init_info,
		m_pGpuObject->GetDevice(), static_cast<uint32_t>(m_MaxFramesInFlight), m_pRenderpassWrapper->GetRenderpass(), commandBuffer);
	// End the single time command buffer
	EndSingleTimeCommands(commandBuffer);
}

void DDM3::VulkanRenderer::AddGraphicsPipeline(const std::string& pipelineName, std::initializer_list<const std::string>&& filePaths, bool hasDepthStencil)
{
	// Add a graphics pipeline trough the pipeline manager
	m_pPipelineManager->AddGraphicsPipeline(m_pGpuObject->GetDevice(), m_pRenderpassWrapper->GetRenderpass(),
		m_pSwapchainWrapper->GetMsaaSamples(), pipelineName, filePaths, hasDepthStencil);
}

VkDevice DDM3::VulkanRenderer::GetDevice()
{
	return m_pGpuObject->GetDevice();
}

VkImageView& DDM3::VulkanRenderer::GetDefaultImageView()
{
	return m_pImageManager->GetDefaultImageView();
}

VkSampler& DDM3::VulkanRenderer::GetSampler()
{
	return m_pImageManager->GetSampler();
}

DDM3::PipelineWrapper* DDM3::VulkanRenderer::GetPipeline(const std::string& name)
{
	return m_pPipelineManager->GetPipeline(name);
}

void DDM3::VulkanRenderer::Render()
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

void DDM3::VulkanRenderer::RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex)
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

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_pSwapchainWrapper->GetExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.388f, 0.588f, 0.929f, 1.0f} };
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

VkCommandBuffer& DDM3::VulkanRenderer::GetCurrentCommandBuffer()
{
	return m_pCommandPoolManager->GetCommandBuffer(m_CurrentFrame);
}

void DDM3::VulkanRenderer::RecreateSwapChain()
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
		commandBuffer, m_pRenderpassWrapper->GetRenderpass());

	// End single time command
	EndSingleTimeCommands(commandBuffer);
}

void DDM3::VulkanRenderer::CreateTexture(Texture& texture, const std::string& textureName)
{
	// Create the image trough the image manager
	m_pImageManager->CreateTextureImage(m_pGpuObject.get(), m_pBufferManager.get(), texture, textureName, m_pCommandPoolManager.get());
	// Create the image view
	texture.imageView = m_pImageManager->CreateImageView(m_pGpuObject->GetDevice(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, texture.mipLevels);
}

void DDM3::VulkanRenderer::CreateCubeTexture(Texture& cubeTexture, const std::initializer_list<const std::string>& textureNames)
{
	// Create a cube texture trough image manager
	m_pImageManager->CreateCubeTexture(m_pGpuObject.get(), m_pBufferManager.get(), cubeTexture, textureNames, m_pCommandPoolManager.get());
}
VkCommandBuffer DDM3::VulkanRenderer::BeginSingleTimeCommands()
{
	return m_pCommandPoolManager->BeginSingleTimeCommands(m_pGpuObject->GetDevice());
}

void DDM3::VulkanRenderer::EndSingleTimeCommands(VkCommandBuffer comandBuffer)
{
	m_pCommandPoolManager->EndSingleTimeCommands(m_pGpuObject.get(), comandBuffer);
}

void DDM3::VulkanRenderer::UpdateUniformBuffer(UniformBufferObject& buffer)
{
	auto pCamera{SceneManager::GetInstance().GetCamera()};

	if (pCamera == nullptr)
		return;

	pCamera->UpdateUniformBuffer(buffer, m_pSwapchainWrapper->GetExtent());
}

DDM3::DescriptorObject* DDM3::VulkanRenderer::GetLightDescriptor()
{
	// Return buffers of the global light object
	return SceneManager::GetInstance().GetGlobalLight()->GetDescriptorObject();
}

void DDM3::VulkanRenderer::AddDefaultPipeline()
{
	m_pPipelineManager->AddDefaultPipeline(m_pGpuObject->GetDevice(), m_pRenderpassWrapper->GetRenderpass(),
		m_MsaaSamples);

}

VkRenderPass DDM3::VulkanRenderer::GetRenderPass() const
{
	return m_pRenderpassWrapper->GetRenderpass();
}

void DDM3::VulkanRenderer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	// Create the buffer trough vulkan utils
	m_pBufferManager->CreateBuffer(m_pGpuObject.get(), size, usage, properties, buffer, bufferMemory);
}

void DDM3::VulkanRenderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	// Copy a buffer trough the bufferManager
	m_pBufferManager->CopyBuffer(m_pGpuObject.get(), m_pCommandPoolManager.get(), srcBuffer, dstBuffer, size);
}

void DDM3::VulkanRenderer::CreateVertexBuffer(std::vector<DDM3::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory)
{
	// Create a vertex buffer trough the buffer manager
	m_pBufferManager->CreateVertexBuffer(m_pGpuObject.get(), m_pCommandPoolManager.get(), vertices, vertexBuffer, vertexBufferMemory);
}

void DDM3::VulkanRenderer::CreateIndexBuffer(std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory)
{
	// Create an index buffer trough the buffer manager
	m_pBufferManager->CreateIndexBuffer(m_pGpuObject.get(), m_pCommandPoolManager.get(), indices, indexBuffer, indexBufferMemory);
}