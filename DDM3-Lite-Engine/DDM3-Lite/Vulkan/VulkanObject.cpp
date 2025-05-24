// VulkanRenderer.cpp

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
#include "Renderers/DefaultRenderer.h"

#include "Components/MeshRenderComponent.h"
#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Components/DirectionalLightComponent.h"

#include "Vulkan/VulkanWrappers/VulkanCore.h"


// Standard library includes
#include <set>
#include <algorithm>

size_t DDM3::VulkanObject::m_MaxFramesInFlight{ static_cast<size_t>(DDM3::ConfigManager::GetInstance().GetInt("MaxFrames"))};
uint32_t DDM3::VulkanObject::m_CurrentFrame{ 0 };

DDM3::VulkanObject::VulkanObject()
{
	// Initialize vulkan core
	m_pVulkanCore = std::make_unique<VulkanCore>();

	m_pBufferCreator = std::make_unique<BufferCreator>();

	m_MsaaSamples = VulkanUtils::GetMaxUsableSampleCount(m_pVulkanCore->GetPhysicalDevice());

	m_pPipelineManager = std::make_unique<PipelineManager>();
}

DDM3::VulkanObject::~VulkanObject()
{
	
}

void DDM3::VulkanObject::AddGraphicsPipeline(const std::string& pipelineName, std::initializer_list<const std::string>&& filePaths, bool hasDepthStencil)
{
	// Add a graphics pipeline trough the pipeline manager
	m_pPipelineManager->AddGraphicsPipeline(m_pVulkanCore->GetDevice(), m_pDefaultRenderer->GetRenderpass(), m_MsaaSamples, pipelineName, filePaths, hasDepthStencil);
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
	return m_pDefaultRenderer->GetDefaultImageView();
}

VkSampler& DDM3::VulkanObject::GetSampler()
{
	return m_pDefaultRenderer->GetSampler();
}

DDM3::PipelineWrapper* DDM3::VulkanObject::GetPipeline(const std::string& name)
{
	return m_pPipelineManager->GetPipeline(name);
}

void DDM3::VulkanObject::Init()
{
	m_pDefaultRenderer = std::make_unique<DefaultRenderer>();

	m_pPipelineManager->AddDefaultPipeline(m_pVulkanCore->GetDevice(), m_pDefaultRenderer->GetRenderpass(), m_MsaaSamples);
}

void DDM3::VulkanObject::Terminate()
{
	m_pDefaultRenderer = nullptr;
}

void DDM3::VulkanObject::Render()
{
	m_pDefaultRenderer->Render();

	++m_CurrentFrame %= m_MaxFramesInFlight;
}

VkCommandBuffer& DDM3::VulkanObject::GetCurrentCommandBuffer()
{
	return m_pDefaultRenderer->GetCurrentCommandBuffer();
}

void DDM3::VulkanObject::CreateTexture(Texture& texture, const std::string& textureName)
{
	// Create the image trough the image manager
	m_pDefaultRenderer->CreateTexture(texture, textureName);
}

void DDM3::VulkanObject::CreateCubeTexture(Texture& cubeTexture, const std::initializer_list<const std::string>& textureNames)
{
	// Create a cube texture trough image manager
	m_pDefaultRenderer->CreateCubeTexture(cubeTexture, textureNames);
}

void DDM3::VulkanObject::UpdateUniformBuffer(UniformBufferObject& buffer)
{
	auto pCamera{SceneManager::GetInstance().GetCamera()};

	if (pCamera == nullptr)
		return;

	pCamera->UpdateUniformBuffer(buffer, m_pDefaultRenderer->GetExtent());
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
	return m_pDefaultRenderer->GetRenderpass();
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

void DDM3::VulkanObject::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	// Create the buffer trough vulkan utils
	m_pBufferCreator->CreateBuffer(m_pVulkanCore->GetGpuObject(), size, usage, properties, buffer, bufferMemory);
}

void DDM3::VulkanObject::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	// Copy a buffer trough the bufferManager
	m_pBufferCreator->CopyBuffer(m_pVulkanCore->GetGpuObject(), m_pDefaultRenderer->GetCommandPoolManager(), srcBuffer, dstBuffer, size);
}

void DDM3::VulkanObject::CreateVertexBuffer(std::vector<DDM3::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory)
{
	// Create a vertex buffer trough the buffer manager
	m_pBufferCreator->CreateVertexBuffer(m_pVulkanCore->GetGpuObject(), m_pDefaultRenderer->GetCommandPoolManager(), vertices, vertexBuffer, vertexBufferMemory);
}

void DDM3::VulkanObject::CreateIndexBuffer(std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory)
{
	// Create an index buffer trough the buffer manager
	m_pBufferCreator->CreateIndexBuffer(m_pVulkanCore->GetGpuObject(), m_pDefaultRenderer->GetCommandPoolManager(), indices, indexBuffer, indexBufferMemory);
}