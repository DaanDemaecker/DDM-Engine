// VulkanObject.cpp

// Header include
#include "VulkanObject.h"

// File includes
#include "Engine/DDM3Engine.h"
#include "Engine/Window.h"
#include "Managers/SceneManager.h"
#include "Managers/ConfigManager.h"

#include "Includes/STBIncludes.h"
#include "Includes/ImGuiIncludes.h"
#include "Includes/GLFWIncludes.h"

#include "VulkanUtils.h"
#include "Utils/Utils.h"

#include "Vulkan/VulkanWrappers/SurfaceWrapper.h"
#include "Vulkan/VulkanManagers/CommandpoolManager.h"
#include "Vulkan/VulkanManagers/BufferCreator.h"
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

#include "Vulkan/VulkanWrappers/VulkanCore.h"


// Standard library includes
#include <set>
#include <algorithm>

uint32_t DDM::VulkanObject::m_MaxFramesInFlight{ static_cast<uint32_t>(DDM::ConfigManager::GetInstance().GetInt("MaxFrames"))};
uint32_t DDM::VulkanObject::m_CurrentFrame{ 0 };

DDM::VulkanObject::VulkanObject()
{
	// Initialize vulkan core
	m_pVulkanCore = std::make_unique<VulkanCore>();

	m_pBufferCreator = std::make_unique<BufferCreator>();

	m_MsaaSamples = VulkanUtils::GetMaxUsableSampleCount(m_pVulkanCore->GetPhysicalDevice());

	m_pPipelineManager = std::make_unique<PipelineManager>();

	m_pCommandPoolManager = std::make_unique<CommandpoolManager>(GetGPUObject(), GetSurface(), m_MaxFramesInFlight);
}

DDM::CommandpoolManager* DDM::VulkanObject::GetCommandPoolManager()
{
	return m_pCommandPoolManager.get();
}

void DDM::VulkanObject::Setup(std::unique_ptr<Renderer> pRenderer)
{
	m_pRenderer = std::move(pRenderer);

	m_pRenderer->AddDefaultPipelines();
}

DDM::VulkanObject::~VulkanObject()
{
	
}

void DDM::VulkanObject::AddGraphicsPipeline(const std::string& pipelineName, std::initializer_list<const std::string>&& filePaths, bool hasDepthStencil, bool writesToDepth, int subpass, RenderpassWrapper* renderpass)
{
	// Add a graphics pipeline trough the pipeline manager
	m_pPipelineManager->AddGraphicsPipeline(m_pVulkanCore->GetDevice(), 
		renderpass == nullptr ? m_pRenderer->GetDefaultRenderpass()->GetRenderpass() : renderpass->GetRenderpass(),
		renderpass == nullptr ? m_pRenderer->GetDefaultRenderpass()->GetSampleCount() : renderpass->GetSampleCount(),
		pipelineName, filePaths, hasDepthStencil, writesToDepth, subpass);
}

VkDevice DDM::VulkanObject::GetDevice()
{
	return m_pVulkanCore->GetDevice();
}

VkPhysicalDevice DDM::VulkanObject::GetPhysicalDevice()
{
	return m_pVulkanCore->GetPhysicalDevice();
}

VkImageView& DDM::VulkanObject::GetDefaultImageView()
{
	return m_pImageManager->GetDefaultImageView();
}

VkSampler& DDM::VulkanObject::GetSampler()
{
	return m_pImageManager->GetSampler();
}

DDM::PipelineWrapper* DDM::VulkanObject::GetPipeline(const std::string& name)
{
	return m_pPipelineManager->GetPipeline(name);
}

void DDM::VulkanObject::Terminate()
{
	m_pRenderer.reset();
}

void DDM::VulkanObject::Render()
{
	m_pRenderer->Render();

	++m_CurrentFrame %= m_MaxFramesInFlight;
}

VkCommandBuffer& DDM::VulkanObject::GetCurrentCommandBuffer()
{
	return m_pCommandPoolManager->GetCommandBuffer(m_CurrentFrame);
}

void DDM::VulkanObject::CreateTexture(Texture& texture, const std::string& textureName)
{
	// Create the image trough the image manager
	m_pImageManager->CreateTextureImage(GetGPUObject(), texture, textureName, GetCommandPoolManager());
	// Create the image view
	texture.imageView = m_pImageManager->CreateImageView(GetDevice(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, texture.mipLevels);
}

void DDM::VulkanObject::CreateCubeTexture(Texture& cubeTexture, const std::initializer_list<const std::string>& textureNames)
{
	// Create a cube texture trough image manager
	m_pImageManager->CreateCubeTexture(GetGPUObject(), cubeTexture, textureNames, GetCommandPoolManager());
}


void DDM::VulkanObject::UpdateUniformBuffer(UniformBufferObject& buffer)
{
	auto pCamera{SceneManager::GetInstance().GetCamera()};

	if (pCamera == nullptr)
		return;

	pCamera->UpdateUniformBuffer(buffer, m_pRenderer->GetExtent());
}

DDM::DescriptorObject* DDM::VulkanObject::GetLightDescriptor()
{
	// Return buffers of the global light object
	return SceneManager::GetInstance().GetGlobalLight()->GetDescriptorObject();
}

DDM::GPUObject* DDM::VulkanObject::GetGPUObject() const
{
	return m_pVulkanCore->GetGpuObject();
}

VkRenderPass DDM::VulkanObject::GetRenderPass() const
{
	return m_pRenderer->GetDefaultRenderpass()->GetRenderpass();
}

int DDM::VulkanObject::GetCurrentFrame()
{
	return m_CurrentFrame;
}

VkSurfaceKHR DDM::VulkanObject::GetSurface()
{
	return m_pVulkanCore->GetSurface();
}

const DDM::QueueObject& DDM::VulkanObject::GetQueueObject()
{
	return m_pVulkanCore->GetQueueObject();
}

VkInstance DDM::VulkanObject::GetVulkanInstance()
{
	return m_pVulkanCore->GetVulkanInstance();
}

VkCommandBuffer DDM::VulkanObject::BeginSingleTimeCommands()
{
	// Create a single time command buffer trough the command pool manager and return it
	return m_pCommandPoolManager->BeginSingleTimeCommands(GetDevice());
}

void DDM::VulkanObject::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	// End the single time command buffer trough the commandpool manager
	m_pCommandPoolManager->EndSingleTimeCommands(GetGPUObject(), commandBuffer);
}

void DDM::VulkanObject::DrawQuad(VkCommandBuffer commandBuffer)
{
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

void DDM::VulkanObject::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	// Create the buffer trough vulkan utils
	m_pBufferCreator->CreateBuffer(GetGPUObject(), size, usage, properties, buffer, bufferMemory);
}

void DDM::VulkanObject::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	// Copy a buffer trough the bufferManager
	m_pBufferCreator->CopyBuffer(GetGPUObject(), m_pCommandPoolManager.get(), srcBuffer, dstBuffer, size);
}

void DDM::VulkanObject::CreateVertexBuffer(std::vector<DDM::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory)
{
	// Create a vertex buffer trough the buffer manager
	m_pBufferCreator->CreateVertexBuffer(GetGPUObject(), m_pCommandPoolManager.get(), vertices, vertexBuffer, vertexBufferMemory);
}

void DDM::VulkanObject::CreateIndexBuffer(std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory)
{
	// Create an index buffer trough the buffer manager
	m_pBufferCreator->CreateIndexBuffer(GetGPUObject(), m_pCommandPoolManager.get(), indices, indexBuffer, indexBufferMemory);
}