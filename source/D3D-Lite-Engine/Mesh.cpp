#include "Mesh.h"
#include "VulkanRenderer.h"
#include "Utils.h"
#include "PipelineWrapper.h"

D3D::Mesh::Mesh(const std::string& filePath)
{
	Utils::LoadModel(filePath, m_Vertices, m_Indices);

	auto& renderer{ VulkanRenderer::GetInstance() };

	renderer.CreateVertexBuffer(m_Vertices, m_VertexBuffer, m_VertexBufferMemory);
	renderer.CreateIndexBuffer(m_Indices, m_IndexBuffer, m_IndexBufferMemory);
}

D3D::Mesh::~Mesh()
{
	Cleanup();
}

void D3D::Mesh::Render(PipelineWrapper* pPipeline, VkDescriptorSet* descriptorSet)
{
	auto commandBuffer{ VulkanRenderer::GetInstance().GetCurrentCommandBuffer() };

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->GetPipeline());

	VkBuffer vertexBuffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->GetPipelineLayout(), 0, 1, descriptorSet, 0, nullptr);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

void D3D::Mesh::Cleanup()
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
