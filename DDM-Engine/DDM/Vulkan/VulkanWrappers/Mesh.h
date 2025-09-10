// Mesh.h
// This class will represent a single mesh, holding an index and vertex buffer

#ifndef _DDM_MESH_
#define _DDM_MESH_

// File includes
#include "Includes/VulkanIncludes.h"
#include "DataTypes/Structs.h"

// Standard library includes
#include <string>
#include <vector>

namespace DDMML
{
	// Class forward declarations
	class Mesh;
}

namespace DDM
{
	// Class forward declarations
	class ResourceManager;
	class PipelineWrapper;

	class Mesh final
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pMesh: ">A pointer to a mesh from the DDMModelLoader lbibrary</param>
		Mesh(DDMML::Mesh* pMesh);

		// Delete default constructor
		Mesh() = delete;

		/// <summary>
		/// Destructor
		/// </summary>
		~Mesh();

		/// <summary>
		/// Rener the model
		/// </summary>
		/// <param name="pPipeline: ">Pointer to the pipeline used for drawing</param>
		/// <param name="descriptorSet: ">Descriptorsets to bind before drawing</param>
		void Render(PipelineWrapper* pPipeline, VkDescriptorSet* descriptorSet);

		/// <summary>
		/// Query wether object is transparant
		/// </summary>
		/// <returns>Bool indicating wether object is transparant</returns>
		bool IsTransparant() const { return m_IsTransparant; }

		/// <summary>
		/// Indicate wether object is transparant
		/// </summary>
		/// <param name="isTransparant: ">New value of transparancy</param>
		void SetTransparancy(bool isTransparant) { m_IsTransparant = isTransparant; }
	private:
		// Friend class declaration
		friend class ResourceManager;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="filePath: ">Filepath to the 3D model file</param>
		Mesh(const std::string& filePath);

		// Indicates wether 
		bool m_IsTransparant{ false };
		
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

		
		/// <summary>
		/// Clean up allocated objects
		/// </summary>
		void Cleanup();

		/// <summary>
		/// Set up index and vertex buffers
		/// </summary>
		void SetupBuffers();
	};
}

#endif // !_DDM_MESH_