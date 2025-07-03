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

uint32_t DDM3::VulkanObject::m_MaxFramesInFlight{ static_cast<uint32_t>(DDM3::ConfigManager::GetInstance().GetInt("MaxFrames"))};
uint32_t DDM3::VulkanObject::m_CurrentFrame{ 0 };

DDM3::VulkanObject::VulkanObject()
{
	// Initialize vulkan core
	m_pVulkanCore = std::make_unique<VulkanCore>();

	m_pBufferCreator = std::make_unique<BufferCreator>();

	m_MsaaSamples = VulkanUtils::GetMaxUsableSampleCount(m_pVulkanCore->GetPhysicalDevice());

	m_pPipelineManager = std::make_unique<PipelineManager>();

	m_pCommandPoolManager = std::make_unique<CommandpoolManager>(GetGPUObject(), GetSurface(), m_MaxFramesInFlight);
}

DDM3::CommandpoolManager* DDM3::VulkanObject::GetCommandPoolManager()
{
	return m_pCommandPoolManager.get();
}

void DDM3::VulkanObject::Setup(std::unique_ptr<Renderer> pRenderer)
{
	m_pRenderer = std::move(pRenderer);

	m_pRenderer->AddDefaultPipelines();
}

DDM3::VulkanObject::~VulkanObject()
{
	
}

void DDM3::VulkanObject::AddGraphicsPipeline(const std::string& pipelineName, std::initializer_list<const std::string>&& filePaths, bool hasDepthStencil, bool writesToDepth, RenderpassWrapper* renderpass)
{
	// Add a graphics pipeline trough the pipeline manager
	m_pPipelineManager->AddGraphicsPipeline(m_pVulkanCore->GetDevice(), 
		renderpass == nullptr ? m_pRenderer->GetDefaultRenderpass()->GetRenderpass() : renderpass->GetRenderpass(),
		renderpass == nullptr ? m_pRenderer->GetDefaultRenderpass()->GetSampleCount() : renderpass->GetSampleCount(),
		pipelineName, filePaths, hasDepthStencil, writesToDepth);
}

VkDevice DDM3::VulkanObject::GetDevice()
{
	return m_pVulkanCore->GetDevice();
}

VkPhysicalDevice DDM3::VulkanObject::GetPhysicalDevice()
{
	return m_pVulkanCore->GetPhysicalDevice();
}

VkImageView& DDM3::VulkanObject::GetDefaultImageView()
{
	return m_pImageManager->GetDefaultImageView();
}

VkSampler& DDM3::VulkanObject::GetSampler()
{
	return m_pImageManager->GetSampler();
}

DDM3::PipelineWrapper* DDM3::VulkanObject::GetPipeline(const std::string& name)
{
	return m_pPipelineManager->GetPipeline(name);
}

void DDM3::VulkanObject::Terminate()
{
	m_pRenderer.reset();
}

void DDM3::VulkanObject::Render()
{
	m_pRenderer->Render();

	++m_CurrentFrame %= m_MaxFramesInFlight;
}

VkCommandBuffer& DDM3::VulkanObject::GetCurrentCommandBuffer()
{
	return m_pCommandPoolManager->GetCommandBuffer(m_CurrentFrame);
}

void DDM3::VulkanObject::CreateTexture(Texture& texture, const std::string& textureName)
{
	// Create the image trough the image manager
	m_pImageManager->CreateTextureImage(GetGPUObject(), texture, textureName, GetCommandPoolManager());
	// Create the image view
	texture.imageView = m_pImageManager->CreateImageView(GetDevice(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, texture.mipLevels);
}

void DDM3::VulkanObject::CreateCubeTexture(Texture& cubeTexture, const std::initializer_list<const std::string>& textureNames)
{
	// Create a cube texture trough image manager
	m_pImageManager->CreateCubeTexture(GetGPUObject(), cubeTexture, textureNames, GetCommandPoolManager());
}


void DDM3::VulkanObject::UpdateUniformBuffer(UniformBufferObject& buffer)
{
	auto pCamera{SceneManager::GetInstance().GetCamera()};

	if (pCamera == nullptr)
		return;

	pCamera->UpdateUniformBuffer(buffer, m_pRenderer->GetExtent());
}

DDM3::DescriptorObject* DDM3::VulkanObject::GetLightDescriptor()
{
	// Return buffers of the global light object
	return SceneManager::GetInstance().GetGlobalLight()->GetDescriptorObject();
}

DDM3::GPUObject* DDM3::VulkanObject::GetGPUObject() const
{
	return m_pVulkanCore->GetGpuObject();
}

VkRenderPass DDM3::VulkanObject::GetRenderPass() const
{
	return m_pRenderer->GetDefaultRenderpass()->GetRenderpass();
}

int DDM3::VulkanObject::GetCurrentFrame()
{
	return m_CurrentFrame;
}

VkSurfaceKHR DDM3::VulkanObject::GetSurface()
{
	return m_pVulkanCore->GetSurface();
}

const DDM3::QueueObject& DDM3::VulkanObject::GetQueueObject()
{
	return m_pVulkanCore->GetQueueObject();
}

VkInstance DDM3::VulkanObject::GetVulkanInstance()
{
	return m_pVulkanCore->GetVulkanInstance();
}

VkCommandBuffer DDM3::VulkanObject::BeginSingleTimeCommands()
{
	// Create a single time command buffer trough the command pool manager and return it
	return m_pCommandPoolManager->BeginSingleTimeCommands(GetDevice());
}

void DDM3::VulkanObject::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	// End the single time command buffer trough the commandpool manager
	m_pCommandPoolManager->EndSingleTimeCommands(GetGPUObject(), commandBuffer);
}

void DDM3::VulkanObject::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	// Create the buffer trough vulkan utils
	m_pBufferCreator->CreateBuffer(GetGPUObject(), size, usage, properties, buffer, bufferMemory);
}

void DDM3::VulkanObject::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	// Copy a buffer trough the bufferManager
	m_pBufferCreator->CopyBuffer(GetGPUObject(), m_pCommandPoolManager.get(), srcBuffer, dstBuffer, size);
}

void DDM3::VulkanObject::CreateVertexBuffer(std::vector<DDM3::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory)
{
	// Create a vertex buffer trough the buffer manager
	m_pBufferCreator->CreateVertexBuffer(GetGPUObject(), m_pCommandPoolManager.get(), vertices, vertexBuffer, vertexBufferMemory);
}

void DDM3::VulkanObject::CreateIndexBuffer(std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory)
{
	// Create an index buffer trough the buffer manager
	m_pBufferCreator->CreateIndexBuffer(GetGPUObject(), m_pCommandPoolManager.get(), indices, indexBuffer, indexBufferMemory);
}