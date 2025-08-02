// QueryPool.cpp

// Header include
#include "QueryPool.h"

// File includes
#include "Vulkan/VulkanObject.h"


DDM3::QueryPool::QueryPool(int queryCount, VkQueryType queryType)
	:m_QueryCount{queryCount}, m_QueryType{queryType}
{
	// Query query pool
	SetupQueryPool();
}

DDM3::QueryPool::~QueryPool()
{
	// Destroy query pool
	vkDestroyQueryPool(VulkanObject::GetInstance().GetDevice(), m_QueryPool, nullptr);
}

void DDM3::QueryPool::ResetPool()
{
	// Reset query pool
	vkResetQueryPool(VulkanObject::GetInstance().GetDevice(), m_QueryPool, 0, m_QueryCount);
}

void DDM3::QueryPool::WriteTimeStamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage)
{
	// Only write timestamp if query type is correct and current query is lower than querycount
	if (m_QueryType == VK_QUERY_TYPE_TIMESTAMP && m_CurrentQuery < m_QueryCount)
	{
		// Write timestamp and increase current query
		vkCmdWriteTimestamp(commandBuffer, pipelineStage, m_QueryPool, m_CurrentQuery);
		++m_CurrentQuery;
	}
}

void DDM3::QueryPool::SetupQueryPool()
{
	// Create info for query pool
	VkQueryPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	createInfo.flags = 0;
	createInfo.queryType = m_QueryType;
	createInfo.queryCount = m_QueryCount;

	// Create query pool
	vkCreateQueryPool(VulkanObject::GetInstance().GetDevice(), &createInfo, nullptr, &m_QueryPool);
}
