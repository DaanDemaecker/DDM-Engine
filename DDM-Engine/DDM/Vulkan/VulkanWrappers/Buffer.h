// Buffer.h

#ifndef _DDM_BUFFER_
#define _DDM_BUFFER_

// File includes
#include "Includes/VulkanIncludes.h"
#include "Vulkan/VulkanObject.h"

// Standard library include
#include <vector>
#include <algorithm>

namespace DDM
{
	// Class forward declaration
	struct Vertex;

	// Templated class so that the user can choose what the descriptor holds
	template <typename T>
	class Buffer
	{
	public:
		// Delete default constructor
		Buffer() = delete;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="data: ">List of data to be stored in buffer</param>
		Buffer(std::vector<T>& data){}

		/// <summary>
		/// Destructor
		/// </summary>
		~Buffer()
		{
			// Destroy the buffer
			vkDestroyBuffer(VulkanObject::GetInstance().GetDevice(), m_Buffer, nullptr);

			// Free buffer memory
			vkFreeMemory(VulkanObject::GetInstance().GetDevice(), m_BufferMemory, nullptr);
		}

		/// <summary>
		/// Get the VkBuffer object
		/// </summary>
		/// <returns>The handle to the buffer</returns>
		VkBuffer GetBuffer() const { return m_Buffer; }

		/// <summary>
		/// Get the size of the vector of data
		/// </summary>
		/// <returns>Size of the data vector</returns>
		size_t GetDataCount() const { return m_Data.size(); }
	private:
		// Data
		std::vector<T> m_Data{};

		// Vulkan buffer
		VkBuffer m_Buffer{};

		// Buffer memory
		VkDeviceMemory m_BufferMemory{};
	};

	// Explicit specialization declaration for Vertex struct
	template <>
	Buffer<Vertex>::Buffer(std::vector<Vertex>& data);

	// Explicit specialization declaration for uint32_t
	template <>
	Buffer<uint32_t>::Buffer(std::vector<uint32_t>& data);
}

#endif // !_DDM_BUFFER_
