// Mesh.cpp

// File includes
#include "Mesh.h"

#include "Vulkan/VulkanObject.h"
#include "Vulkan/VulkanWrappers/PipelineWrapper.h"

#include "Engine/DDMModelLoader.h"

#include "Includes/DDMModelLoaderIncludes.h"


DDM::Mesh::Mesh(DDMML::Mesh* pMesh)
{
	auto& vertices = pMesh->GetVertices();
	auto& indices = pMesh->GetIndices();

	std::vector<Vertex> convertedVertices{};

	DDMModelLoader::GetInstance().ConvertVertices(vertices, convertedVertices);

	m_pIndexBuffer = std::make_unique<Buffer<uint32_t>>(indices);	
	m_pVertexBuffer = std::make_unique<Buffer<Vertex>>(convertedVertices);


	m_IsTransparant = pMesh->GetIsTransparant();
}

DDM::Mesh::Mesh(const std::string& filePath)
{

	std::vector<uint32_t> indices{};
	std::vector<Vertex> vertices{};

	// Load the vertices and indices
	DDM::DDMModelLoader::GetInstance().LoadModel(filePath, vertices, indices);


	m_pIndexBuffer = std::make_unique<Buffer<uint32_t>>(indices);
	m_pVertexBuffer = std::make_unique<Buffer<Vertex>>(vertices);
}

DDM::Mesh::~Mesh()
{
	
}

void DDM::Mesh::Render(PipelineWrapper* pPipeline, VkDescriptorSet* descriptorSet)
{
	// Get current commandbuffer
	auto commandBuffer{ VulkanObject::GetInstance().GetCurrentCommandBuffer() };

	if (pPipeline != nullptr)
	{
		// Bind pipeline
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->GetPipeline());
	}

	// Set and bind vertex buffer
	VkBuffer vertexBuffers[] = { m_pVertexBuffer->GetBuffer()};
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	// Bind index buffer
	vkCmdBindIndexBuffer(commandBuffer, m_pIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

	if (pPipeline != nullptr && descriptorSet != nullptr)
	{
		// Bind descriptor sets
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->GetPipelineLayout(), 0, 1, descriptorSet, 0, nullptr);
	}

	// Draw
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_pIndexBuffer->GetDataCount()), 1, 0, 0, 0);
}
