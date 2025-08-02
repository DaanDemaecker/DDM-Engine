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
	// Reset query pool and current query
	vkResetQueryPool(VulkanObject::GetInstance().GetDevice(), m_QueryPool, 0, m_QueryCount);

	m_CurrentQuery = 0;
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

void DDM3::QueryPool::PrintTimestamps()
{
	// Only print if query type is correct
	if (m_QueryType == VK_QUERY_TYPE_TIMESTAMP)
	{
		// Create vector of timestamps of size querycount
		std::vector<uint64_t> timeStamps(m_QueryCount * 2);

		// Query pool results
		vkGetQueryPoolResults(
			VulkanObject::GetInstance().GetDevice(),
			m_QueryPool,
			0,
			m_QueryCount,
			sizeof(uint64_t) * timeStamps.size(),
			timeStamps.data(),
			sizeof(uint64_t) * 2,
			VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WITH_AVAILABILITY_BIT
		);

		auto physicalDevice = VulkanObject::GetInstance().GetPhysicalDevice();

		VkPhysicalDeviceProperties deviceProperties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

		float timestampPeriod = deviceProperties.limits.timestampPeriod;

		// Loop trough timestamps and print them
		for (uint64_t index{}; index < timeStamps.size() - 2; index += 2)
		{
			uint64_t rawTimeStamp = timeStamps[index];
			double timestampNs = rawTimeStamp * timestampPeriod;

			uint64_t avail = timeStamps[index + 1];

			if (avail == 0)
			{
				std::cout << "Timestamp " << index/2 << ": not yet available.\n";
				continue;
			}

			if (index > 0)
			{
				uint64_t rawTimeStamp2 = timeStamps[index - 2];
				double timestampNs2 = rawTimeStamp2 * timestampPeriod;

				double deltaTime = rawTimeStamp - rawTimeStamp2;
				deltaTime *= timestampPeriod;

				std::cout << "DeltaTime: " << deltaTime << " nanoseconds" << std::endl;
			}

			std::cout << "Timestamp " << index / 2 << ": " << timestampNs << " nanoseconds" << std::endl;


		}
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
