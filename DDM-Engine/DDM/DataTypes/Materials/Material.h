// Material.h
// This class will serve as the baseclass for all materials

#ifndef _MATERIAL_
#define _MATERIAL_

// File includes
#include "DataTypes/Structs.h"

// Standard library includes
#include <iostream>

namespace DDM
{
	// Class forward declarations
	class DescriptorPoolWrapper;
	class MeshRenderComponent;
	class PipelineWrapper;
	class DescriptorObject;

	class Material
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pipelineName: ">Name of the graphics pipeline to be used</param>
		Material(const std::string& pipelineName = "Default");

		/// <summary>
		/// Default destructor
		/// </summary>
		virtual ~Material() = default;

		// Rule of five
		Material(Material& other) = delete;
		Material(Material&& other) = delete;

		Material& operator=(Material& other) = delete;
		Material& operator=(Material&& other) noexcept;
		
		/// <summary>
		/// Function to render ImGui windows
		/// </summary>
		virtual void OnGUI() {}

		/// <summary>
		/// Get the pipeline that is used by this material
		/// </summary>
		/// <returns>Pointer to the pipeline wrapper</returns>
		PipelineWrapper* GetPipeline();

		// Create the descriptorsets
		// Parameters:
		//     pModel: the model that the descriptorsets belong to
		//     descriptorSets: vector of descriptorsets that have to be created

		/// <summary>
		/// Create the descriptorsets
		/// </summary>
		/// <param name="pModel: ">Pointer to the mesrendercomponent that the descriptorsets belong to</param>
		/// <param name="descriptorSets: ">Reference to the vector that holds the to be created descriptorsets</param>
		void CreateDescriptorSets(MeshRenderComponent* pModel, std::vector<VkDescriptorSet>& descriptorSets);

		// Update the descriptorsets
		// Parameters:
		//     descriptorsets: the descriptorsets that should be updated
		//     descriptorObjects: a vector of pointers to descriptorobjects in the same order as the shader code

		/// <summary>
		/// Update the descriptorsets
		/// </summary>
		/// <param name="descriptorSets: ">Reference to the vector holding the descriptorsets that should be updated</param>
		/// <param name="descriptorObjects: ">List of descriptorobjects to be used to update descrptorsets</param>
		virtual void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects);

		/// <summary>
		/// Get the descriptorset layout from the pipeline
		/// </summary>
		/// <returns>Handle ot the descriptorset layout</returns>
		VkDescriptorSetLayout GetDescriptorLayout();

		/// <summary>
		/// Get the descriptor pool that is used for the pipeline
		/// </summary>
		/// <returns>Pointer to descriptor pool that is used in this pipeline</returns>
		DescriptorPoolWrapper* GetDescriptorPool();

		/// <summary>
		/// Indicates wether descriptorsets should be updated
		/// </summary>
		/// <returns>Boolean indicateing wether descriptorsets should be updated</returns>
		bool ShouldUpdateDescriptorSets() const { return m_ShouldUpdateDescriptorSets; }
	protected:
		// The pipeline pair that is used for this material
		PipelineWrapper* m_pPipeline{};

		// Indicates wether descriptorsets should be updated
		bool m_ShouldUpdateDescriptorSets{true};
	};
}
#endif // !_MATERIAL_