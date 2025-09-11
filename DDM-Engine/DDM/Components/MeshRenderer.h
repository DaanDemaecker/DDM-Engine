// MeshRenderer.h
// This component will render an attached mesh

#ifndef _DDM_MESH_RENDERER_
#define _DDM_MESH_RENDERER_

// Parent include
#include "BaseClasses/Component.h"

// File includes
#include "DataTypes/Structs.h"
#include "DataTypes/DescriptorObjects/UboDescriptorObject.h"

// Standard library includes
#include <memory>
#include <vector>

// Forward declaration for mesh class from DDMModelLoader
namespace DDMML
{
	class Mesh;
}

namespace DDM
{
	// Class forward declarations
	class Material;
	class Mesh;
	class PipelineWrapper;

	class MeshRenderComponent : public Component
	{
	public:
		// Default constructor
		MeshRenderComponent();

		// Default destructor
		virtual ~MeshRenderComponent() = default;

		// Delete copy and move operations
		MeshRenderComponent(const MeshRenderComponent& other) = delete;
		MeshRenderComponent(MeshRenderComponent&& other) = delete;

		MeshRenderComponent& operator=(const MeshRenderComponent& other) = delete;
		MeshRenderComponent& operator=(MeshRenderComponent&& other) = delete;

		/// <summary>
		/// Early update function
		/// </summary>
		virtual void EarlyUpdate() override;

		/// <summary>
		/// Set the mesh to be rendered
		/// </summary>
		/// <param name="pMesh: ">Pointer to Mesh object to be rendered</param>
		void SetMesh(std::shared_ptr<Mesh> pMesh);

		/// <summary>
		/// Set the mesh to be rendered
		/// </summary>
		/// <param name="pMesh: ">Pointer to DDMML Mesh object</param>
		void SetMesh(DDMML::Mesh* pMesh);

		/// <summary>
		/// Set material
		/// </summary>
		/// <param name="pMaterial: ">Pointer to material to be used</param>
		void SetMaterial(std::shared_ptr<Material> pMaterial);

		/// <summary>
		/// Render depth
		/// </summary>
		virtual void RenderDepth() override;

		/// <summary>
		/// General render function
		/// </summary>
		virtual void Render() override;

		/// <summary>
		/// Render transparant objects
		/// </summary>
		virtual void RenderTransparancy() override;

		/// <summary>
		/// Render the ImGui elements
		/// </summary>
		virtual void OnGUI() override;

		/// <summary>
		/// Create the descriptorsets
		/// </summary>
		void CreateDescriptorSets();

	protected:
		// Indicates if object is transparant
		bool m_IsTransparant{ false };

		// Indicates if component has been initialized
		bool m_Initialized{ false };

		// Vector for Uniform Buffer Objects
		std::vector<UniformBufferObject> m_Ubos{};

		// Vector of dirty flags for UBOs
		std::vector<bool> m_UboChanged{};

		// Pointer to UBO descriptor object
		std::unique_ptr<DDM::UboDescriptorObject<UniformBufferObject>> m_pUboDescriptorObject{};

		// Indicates if descriptorsets should be (re)created
		bool m_ShouldCreateDescriptorSets{ false };

		// Pointer to mesh to be rendered
		std::shared_ptr<Mesh> m_pMesh{};

		// Pointer to material
		std::shared_ptr<Material> m_pMaterial{};

		// Vector of descriptorsets
		std::vector<VkDescriptorSet> m_DescriptorSets{};

		// Vector of descriptorsets for depth rendering
		std::vector<VkDescriptorSet> m_DepthDescriptorSets{};

		/// <summary>
		/// Create the uniform buffers
		/// </summary>
		void CreateUniformBuffers();

		/// <summary>
		/// Update the descriptorsets
		/// </summary>
		void UpdateDescriptorSets();

		/// <summary>
		/// Update the uniform buffer objects
		/// </summary>
		/// <param name="frame: ">index of the current frame in flight</param>
		void UpdateUniformBuffer(uint32_t frame);

		/// <summary>
		/// Get the pipeline wrapper used for rendering
		/// </summary>
		/// <returns>Pointer to the pipeline wrapper</returns>
		PipelineWrapper* GetPipeline();

		/// <summary>
		/// Create the descriptor sets for depth rendering
		/// </summary>
		void CreateDepthDescriptorSets();

		/// <summary>
		/// Update the descriptor sets for depth rendering
		/// </summary>
		void UpdateDepthDescriptorSets();

		/// <summary>
		/// Get a pointer to the depth pipeline
		/// </summary>
		/// <returns>Pointer to the depth pipeline</returns>
		PipelineWrapper* GetDepthPipeline();
	};
}
#endif // !_DDM_MESH_RENDERER_