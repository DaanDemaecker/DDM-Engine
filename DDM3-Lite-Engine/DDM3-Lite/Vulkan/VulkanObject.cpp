// VulkanRenderer.cpp

// Header include
#include "VulkanObject.h"

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
#include "Renderers/DefaultRenderer.h"

#include "Components/MeshRenderComponent.h"
#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Components/DirectionalLightComponent.h"


// Standard library includes
#include <set>
#include <algorithm>

DDM3::VulkanObject::VulkanObject()
{
	InitVulkan();
}

DDM3::VulkanObject::~VulkanObject()
{
	m_pGpuObject->WaitIdle();

	m_pSurfaceWrapper->Cleanup(GetVulkanInstance());
}

void DDM3::VulkanObject::InitVulkan()
{
	m_pInstanceWrapper = std::make_unique<InstanceWrapper>();

	m_pSurfaceWrapper = std::make_unique<SurfaceWrapper>(m_pInstanceWrapper->GetInstance());

	m_pGpuObject = std::make_unique<GPUObject>(m_pInstanceWrapper.get(), m_pSurfaceWrapper->GetSurface());
}

void DDM3::VulkanObject::AddGraphicsPipeline(const std::string& pipelineName, std::initializer_list<const std::string>&& filePaths, bool hasDepthStencil)
{
	// Add a graphics pipeline trough the pipeline manager
	m_pDefaultRenderer->AddGraphicsPipeline(pipelineName, filePaths, hasDepthStencil);
}

VkDevice DDM3::VulkanObject::GetDevice()
{
	return m_pGpuObject->GetDevice();
}

VkPhysicalDevice DDM3::VulkanObject::GetPhysicalDevice()
{
	return m_pGpuObject->GetPhysicalDevice();
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
	return m_pDefaultRenderer->GetPipeline(name);
}

void DDM3::VulkanObject::Init()
{
	m_pDefaultRenderer = std::make_unique<DefaultRenderer>();
}

void DDM3::VulkanObject::Terminate()
{
	m_pGpuObject->WaitIdle();
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
	return m_pSurfaceWrapper->GetSurface();
}

const DDM3::QueueObject& DDM3::VulkanObject::GetQueueObject()
{
	return m_pGpuObject->GetQueueObject();
}

VkInstance DDM3::VulkanObject::GetVulkanInstance()
{
	return m_pInstanceWrapper->GetInstance();
}

void DDM3::VulkanObject::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	// Create the buffer trough vulkan utils
	m_pDefaultRenderer->CreateBuffer(size, usage, properties, buffer, bufferMemory);
}

void DDM3::VulkanObject::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	// Copy a buffer trough the bufferManager
	m_pDefaultRenderer->CopyBuffer(srcBuffer, dstBuffer, size);
}

void DDM3::VulkanObject::CreateVertexBuffer(std::vector<DDM3::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory)
{
	// Create a vertex buffer trough the buffer manager
	m_pDefaultRenderer->CreateVertexBuffer(vertices, vertexBuffer, vertexBufferMemory);
}

void DDM3::VulkanObject::CreateIndexBuffer(std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory)
{
	// Create an index buffer trough the buffer manager
	m_pDefaultRenderer->CreateIndexBuffer(indices, indexBuffer, indexBufferMemory);
}