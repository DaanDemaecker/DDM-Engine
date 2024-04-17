#ifndef MeshIncluded
#define MeshIncluded

// File includes
#include "VulkanIncludes.h"
#include "Structs.h"

// Standard library includes
#include <string>
#include <vector>

namespace D3D
{
	// Class forward declarations
	class PipelineWrapper;

	class Mesh final
	{
	public:
		Mesh(const std::string& filePath);

		~Mesh();

		void Render(PipelineWrapper* pPipeline, VkDescriptorSet* descriptorSet);
	private:
		std::vector<Vertex> m_Vertices{};
		std::vector<uint32_t> m_Indices{};

		//Vertex buffer
		VkBuffer m_VertexBuffer{};
		VkDeviceMemory m_VertexBufferMemory{};

		//Index buffer
		VkBuffer m_IndexBuffer{};
		VkDeviceMemory m_IndexBufferMemory{};

		void Cleanup();
	};
}

#endif // !MeshIncluded