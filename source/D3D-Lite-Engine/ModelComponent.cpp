#include "ModelComponent.h"
#include "VulkanRenderer.h"
#include "Material.h"
#include "TransformComponent.h"
#include "Utils.h"
#include "DescriptorPoolWrapper.h"

D3D::ModelComponent::ModelComponent()
{
	m_pMaterial = std::make_shared<D3D::Material>();
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
	CreateVertexBuffer();
	CreateIndexBuffer();
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

void D3D::ModelComponent::CreateVertexBuffer()
{
	auto& renderer{ VulkanRenderer::GetInstance() };
	auto device = renderer.GetDevice();

	VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;

	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Vertices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(device, stagingBufferMemory);

	renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

	renderer.CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void D3D::ModelComponent::CreateIndexBuffer()
{
	auto& renderer{ VulkanRenderer::GetInstance() };
	auto device = renderer.GetDevice();

	VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Indices.data(), static_cast<size_t>(bufferSize));

	renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

	renderer.CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void D3D::ModelComponent::CreateUniformBuffers()
{
	auto& renderer = VulkanRenderer::GetInstance();

	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	auto maxFrames{renderer.GetMaxFrames()};

	m_UboBuffers.resize(maxFrames);
	m_UbosMemory.resize(maxFrames);
	m_UbosMapped.resize(maxFrames);

	m_Ubos.resize(maxFrames);


	for (size_t i = 0; i < maxFrames; ++i)
	{
		renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_UboBuffers[i], m_UbosMemory[i]);

		vkMapMemory(renderer.GetDevice(), m_UbosMemory[i], 0, bufferSize, 0, &m_UbosMapped[i]);
	}
}

void D3D::ModelComponent::CreateDescriptorSets()
{
	m_pMaterial->CreateDescriptorSets(this, m_DescriptorSets);
	UpdateDescriptorSets();
}

void D3D::ModelComponent::UpdateDescriptorSets()
{
	m_pMaterial->UpdateDescriptorSets(m_UboBuffers, m_DescriptorSets);
}

void D3D::ModelComponent::UpdateUniformBuffer(uint32_t frame)
{
	auto transform{ GetTransform() };

	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform->GetWorldPosition());

	glm::mat4 rotationMatrix = glm::mat4_cast(transform->GetWorldRotation());

	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), transform->GetWorldScale());

	m_Ubos[frame].model = translationMatrix * rotationMatrix * scalingMatrix;

	VulkanRenderer::GetInstance().UpdateUniformBuffer(m_Ubos[frame]);

	memcpy(m_UbosMapped[frame], &m_Ubos[frame], sizeof(m_Ubos[frame]));
}

D3D::PipelinePair& D3D::ModelComponent::GetPipeline()
{
	if (m_pMaterial != nullptr)
	{
		return m_pMaterial->GetPipeline();
	}

	return VulkanRenderer::GetInstance().GetPipeline();
}

void D3D::ModelComponent::Cleanup()
{
	auto& renderer{ D3D::VulkanRenderer::GetInstance() };
	auto device = renderer.GetDevice();

	vkDeviceWaitIdle(device);

	vkDestroyBuffer(device, m_IndexBuffer, nullptr);
	vkFreeMemory(device, m_IndexBufferMemory, nullptr);

	vkDestroyBuffer(device, m_VertexBuffer, nullptr);
	vkFreeMemory(device, m_VertexBufferMemory, nullptr);

	for (size_t i = 0; i < renderer.GetMaxFrames(); ++i)
	{
		vkDestroyBuffer(device, m_UboBuffers[i], nullptr);
		vkFreeMemory(device, m_UbosMemory[i], nullptr);
	}
}