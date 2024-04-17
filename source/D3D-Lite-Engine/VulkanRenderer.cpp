#include "VulkanRenderer.h"
#include "D3DEngine.h"
#include "Utils.h"
#include <set>
#include <algorithm>

#include "SceneManager.h"
#include "MeshRenderComponent.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "SurfaceWrapper.h"
#include "CommandpoolManager.h"
#include "BufferManager.h"
#include "ImageManager.h"
#include "SwapchainWrapper.h"
#include "RenderpassWrapper.h"
#include "PipelineManager.h"
#include "PipelineWrapper.h"


#include "STBIncludes.h"
#include "ImGuiIncludes.h"
#include "Window.h"

#include "InstanceWrapper.h"
#include "GPUObject.h"
#include "ImGuiWrapper.h"
#include "SyncObjectManager.h"

#include "DirectionalLightComponent.h"

#include "VulkanUtils.h"

D3D::VulkanRenderer::VulkanRenderer()
{
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

	m_pPipelineManager->Cleanup(device);


	m_pRenderpassWrapper->cleanup(device);

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
	m_pImGuiWrapper = std::make_unique<D3D::ImGuiWrapper>(init_info, m_pRenderpassWrapper->GetRenderpass(),
		commandBuffer, m_pGpuObject->GetDevice(), static_cast<uint32_t>(m_MaxFramesInFlight));
	// End the single time command buffer
	EndSingleTimeCommands(commandBuffer);
}

void D3D::VulkanRenderer::AddGraphicsPipeline(const std::string& pipelineName, std::initializer_list<const std::string>&& filePaths, bool hasDepthStencil)
{
	// Add a graphics pipeline trough the pipeline manager
	m_pPipelineManager->AddGraphicsPipeline(m_pGpuObject->GetDevice(), m_pRenderpassWrapper->GetRenderpass(),
		m_pSwapchainWrapper->GetMsaaSamples(), pipelineName, filePaths, hasDepthStencil);
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

D3D::PipelineWrapper* D3D::VulkanRenderer::GetPipeline(const std::string& name)
{
	return m_pPipelineManager->GetPipeline(name);
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

	SceneManager::GetInstance().Render();

	// Render the ImGui
	m_pImGuiWrapper->Render(commandBuffer);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void D3D::VulkanRenderer::Render(D3D::MeshRenderComponent* pModel, VkCommandBuffer& commandBuffer, const VkDescriptorSet* descriptorSet, const PipelineWrapper* pipeline)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

	VkBuffer vertexBuffers[] = { pModel->GetVertexBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer, pModel->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, descriptorSet, 0, nullptr);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(pModel->GetIndexAmount()), 1, 0, 0, 0);
}

VkCommandBuffer& D3D::VulkanRenderer::GetCurrentCommandBuffer()
{
	return m_pCommandPoolManager->GetCommandBuffer(m_CurrentFrame);
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
		commandBuffer, m_pRenderpassWrapper->GetRenderpass());

	// End single time command
	EndSingleTimeCommands(commandBuffer);
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

	pCamera->UpdateUniformBuffer(buffer, m_pSwapchainWrapper->GetExtent());
}

D3D::DescriptorObject* D3D::VulkanRenderer::GetLightDescriptor()
{
	// Return buffers of the global light object
	return SceneManager::GetInstance().GetGlobalLight()->GetDescriptorObject();
}

void D3D::VulkanRenderer::AddDefaultPipeline()
{
	m_pPipelineManager->AddDefaultPipeline(m_pGpuObject->GetDevice(), m_pRenderpassWrapper->GetRenderpass(),
		m_MsaaSamples);

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