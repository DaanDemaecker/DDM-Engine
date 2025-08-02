// QueryPool.h
// This class will serve as a wrapper for a VkQueryPool object 


#ifndef _QUERY_POOL_
#define _QUERY_POOL_

// File includes
#include "Includes/VulkanIncludes.h"
#include "Includes/GLMIncludes.h"

// Standard library includes
#include <string>



namespace DDM3
{
	class QueryPool final
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="queryCount: ">Amount of possible queries, default: 64</param>
		/// <param name="queryType: ">Vulkan type of queries, default: VK_QUERY_TYPE_TIMESTAMP</param>
		QueryPool(int queryCount = 64, VkQueryType queryType = VK_QUERY_TYPE_TIMESTAMP);

		// Destructor
		~QueryPool();

		// Delete copy and move operations
		QueryPool(QueryPool&) = delete;
		QueryPool(QueryPool&&) = delete;

		QueryPool& operator=(QueryPool&) = delete;
		QueryPool& operator=(QueryPool&&) = delete;

		/// <summary>
		/// Reset the query pool
		/// </summary>
		void ResetPool();

		/// <summary>
		/// Write a timestamp
		/// </summary>
		/// <param name="commandBuffer: ">Current commandbuffer to write timestamp to</param>
		/// <param name="pipelineStage: ">Vulkan pipeline stage to put timestamp in, default: TOP_OF_PIPE</param>
		void WriteTimeStamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

		void PrintTimestamps();
	private:
		// Vulkan query pool object
		VkQueryPool m_QueryPool{};

		// Current query
		int m_CurrentQuery{};

		// Query count and type for creation and reset
		const int m_QueryCount{};
		const VkQueryType m_QueryType{};

		/// <summary>
		/// Querypool creation function
		/// </summary>
		void SetupQueryPool();
	};
}


#endif // !_QUERY_POOL_
