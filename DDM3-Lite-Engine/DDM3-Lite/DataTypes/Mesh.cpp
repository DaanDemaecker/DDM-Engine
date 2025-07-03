// Mesh.cpp

// File includes
#include "Mesh.h"
#include "Vulkan/VulkanObject.h"
#include "../Vulkan/VulkanWrappers/PipelineWrapper.h"
#include "Engine/DDMModelLoader.h"
#include "DDMModelLoader/DDMModelLoader.h"


DDM3::Mesh::Mesh(DDMML::Mesh* pMesh)
{
	m_Vertices.clear();
	m_Indices.clear();

	auto& vertices = pMesh->GetVertices();
	auto& indices = pMesh->GetIndices();

	DDMModelLoader::GetInstance().ConvertVertices(vertices, m_Vertices);

	m_Indices.resize(indices.size());

	std::copy(indices.begin(), indices.end(), m_Indices.begin());

	SetupBuffers();
}

DDM3::Mesh::Mesh(const std::string& filePath)
{
	// Load the vertices and indices
	DDM3::DDMModelLoader::GetInstance().LoadModel(filePath, m_Vertices, m_Indices);

	SetupBuffers();
}

DDM3::Mesh::~Mesh()
{
	// Call cleanup function
	Cleanup();
}

void DDM3::Mesh::Render(PipelineWrapper* pPipeline, VkDescriptorSet* descriptorSet)
{
	// Get current commandbuffer
	auto commandBuffer{ VulkanObject::GetInstance().GetCurrentCommandBuffer() };

	if (pPipeline != nullptr)
	{
		// Bind pipeline
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->GetPipeline());
	}

	// Set and bind vertex buffer
	VkBuffer vertexBuffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	// Bind index buffer
	vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

	if (pPipeline != nullptr && descriptorSet != nullptr)
	{
		// Bind descriptor sets
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->GetPipelineLayout(), 0, 1, descriptorSet, 0, nullptr);
	}

	// Draw
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

void DDM3::Mesh::Cleanup()
{
	// Get reference to renderer
	auto& renderer = DDM3::VulkanObject::GetInstance();
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

void DDM3::Mesh::SetupBuffers()
{	
	// Get reference to the renderer
	auto& renderer{ VulkanObject::GetInstance() };

	// Create vertex and index buffer
	renderer.CreateVertexBuffer(m_Vertices, m_VertexBuffer, m_VertexBufferMemory);
	renderer.CreateIndexBuffer(m_Indices, m_IndexBuffer, m_IndexBufferMemory);
}
