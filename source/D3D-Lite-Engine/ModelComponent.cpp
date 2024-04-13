#include "ModelComponent.h"
#include "VulkanRenderer.h"
#include "Material.h"
#include "TransformComponent.h"
#include "Utils.h"
#include "DescriptorPoolWrapper.h"

D3D::ModelComponent::ModelComponent()
{
	m_pMaterial = std::make_shared<D3D::Material>();

	m_pUboDescriptorObject = std::make_unique<D3D::UboDescriptorObject<UniformBufferObject>>();
}

D3D::ModelComponent::~ModelComponent()
{
	if (m_Initialized)
	{
		Cleanup();
	}
}

void D3D::ModelComponent::LoadModel(const std::string& textPath)
{
	if (m_Initialized)
	{
		m_Initialized = false;
		Cleanup();
	}

	Utils::LoadModel(textPath, m_Vertices, m_Indices);

	auto& renderer{ VulkanRenderer::GetInstance() };

	renderer.CreateVertexBuffer(m_Vertices, m_VertexBuffer, m_VertexBufferMemory);
	renderer.CreateIndexBuffer(m_Indices, m_IndexBuffer, m_IndexBufferMemory);

	CreateUniformBuffers();
	CreateDescriptorSets();

	m_Initialized = true;
}

void D3D::ModelComponent::SetMaterial(std::shared_ptr<Material> pMaterial)
{
	m_pMaterial->GetDescriptorPool()->RemoveModel(this);
	m_pMaterial = pMaterial;
	CreateDescriptorSets();
}

void D3D::ModelComponent::Render()
{
	if (!m_Initialized)
		return;

	auto& renderer{ VulkanRenderer::GetInstance() };

	auto frame{ renderer.GetCurrentFrame() };

	UpdateUniformBuffer(frame);

	renderer.Render(this, renderer.GetCurrentCommandBuffer(), &m_DescriptorSets[frame], GetPipeline());
}

void D3D::ModelComponent::CreateUniformBuffers()
{
	// Get reference to renderer
	auto& renderer = VulkanRenderer::GetInstance();
	// Get amount of frames
	auto frames = renderer.GetMaxFrames();

	// Resize ubos to amount of frames
	m_Ubos.resize(frames);
	// Resize dirty flages to amount of frames
	m_UboChanged.resize(frames);
}

void D3D::ModelComponent::CreateDescriptorSets()
{
	// Create descriptorsets
	m_pMaterial->CreateDescriptorSets(this, m_DescriptorSets);
	// Update descriptors
	UpdateDescriptorSets();
}

void D3D::ModelComponent::UpdateDescriptorSets()
{
	std::vector<DescriptorObject*> descriptors{ m_pUboDescriptorObject.get() };

	// Update descriptorsets
	m_pMaterial->UpdateDescriptorSets(m_DescriptorSets, descriptors);
}

void D3D::ModelComponent::UpdateUniformBuffer(uint32_t frame)
{
	auto transform{ GetTransform() };

	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform->GetWorldPosition());

	glm::mat4 rotationMatrix = glm::mat4_cast(transform->GetWorldRotation());

	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), transform->GetWorldScale());

	m_Ubos[frame].model = translationMatrix * rotationMatrix * scalingMatrix;

	VulkanRenderer::GetInstance().UpdateUniformBuffer(m_Ubos[frame]);

	m_pUboDescriptorObject->UpdateUboBuffer(m_Ubos[frame], frame);
}

D3D::PipelineWrapper* D3D::ModelComponent::GetPipeline()
{
	if (m_pMaterial != nullptr)
	{
		return m_pMaterial->GetPipeline();
	}

	return VulkanRenderer::GetInstance().GetPipeline();
}

void D3D::ModelComponent::Cleanup()
{
	// Get reference to renderer
	auto& renderer = D3D::VulkanRenderer::GetInstance();
	// Get reference to device
	auto device = renderer.GetDevice();

	// Wait until device is idle
	vkDeviceWaitIdle(device);

	// Destroy index buffer
	vkDestroyBuffer(device, m_IndexBuffer, nullptr);
	// Free index buffer memory
	vkFreeMemory(device, m_IndexBufferMemory, nullptr);

	// Destroy vertex buffer
	vkDestroyBuffer(device, m_VertexBuffer, nullptr);
	// Free vertex buffer
	vkFreeMemory(device, m_VertexBufferMemory, nullptr);
}