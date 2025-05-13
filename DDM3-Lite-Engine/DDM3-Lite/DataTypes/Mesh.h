#ifndef MeshIncluded
#define MeshIncluded

// File includes
#include "../Includes/VulkanIncludes.h"
#include "Structs.h"

// Standard library includes
#include <string>
#include <vector>

namespace DDMML
{
	// Class forward declarations
	class Mesh;
}

namespace DDM3
{
	// Class forward declarations
	class ResourceManager;
	class PipelineWrapper;

	class Mesh final
	{
	public:
		Mesh(DDMML::Mesh* pMesh);

		// Delete default constructor
		Mesh() = delete;

		// Destructor
		~Mesh();

		// Render the model
		// Parameters:
		//     pPipeline: the pipeline to bind before drawing
		//     descriptorSet: the descriptorset to bind before drawing
		void Render(PipelineWrapper* pPipeline, VkDescriptorSet* descriptorSet);
	private:
		// Friend class declaration
		friend class ResourceManager;

		// Constructor
		// Parameters:
		//     filePath: the filepath to the 3D model
		Mesh(const std::string& filePath);

		// Vector of vertices
		std::vector<Vertex> m_Vertices{};

		// Vertex buffer
		VkBuffer m_VertexBuffer{};

		// Vertex buffer memory
		VkDeviceMemory m_VertexBufferMemory{};

		// Vector of indices
		std::vector<uint32_t> m_Indices{};

		// Index buffer
		VkBuffer m_IndexBuffer{};

		// Index buffer memory
		VkDeviceMemory m_IndexBufferMemory{};

		// Clean up all allocated objects
		void Cleanup();

		/// <summary>
		/// Set up index and vertex buffers
		/// </summary>
		void SetupBuffers();
	};
}

#endif // !MeshIncluded