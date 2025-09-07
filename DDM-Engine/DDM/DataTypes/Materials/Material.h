#ifndef MaterialIncluded
#define MaterialIncluded

#include "DataTypes/Structs.h"
#include <iostream>

namespace DDM
{
	class DescriptorPoolWrapper;
	class MeshRenderComponent;
	class PipelineWrapper;
	class DescriptorObject;

	class Material
	{
	public:
		Material(const std::string& pipelineName = "Default");
		virtual ~Material() = default;

		Material(Material& other) = delete;
		Material(Material&& other) = delete;

		Material& operator=(Material& other) = delete;
		Material& operator=(Material&& other);

		virtual void OnGUI() {}

		// Get the pipeline that is used by this material
		PipelineWrapper* GetPipeline();

		// Create the descriptorsets
		// Parameters:
		//     pModel: the model that the descriptorsets belong to
		//     descriptorSets: vector of descriptorsets that have to be created
		void CreateDescriptorSets(MeshRenderComponent* pModel, std::vector<VkDescriptorSet>& descriptorSets);

		// Update the descriptorsets
		// Parameters:
		//     descriptorsets: the descriptorsets that should be updated
		//     descriptorObjects: a vector of pointers to descriptorobjects in the same order as the shader code
		virtual void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects);


		VkDescriptorSetLayout GetDescriptorLayout();

		DescriptorPoolWrapper* GetDescriptorPool();

		bool ShouldUpdateDescriptorSets() const { return m_ShouldUpdateDescriptorSets; }
	protected:
		// The pipeline pair that is used for this material
		PipelineWrapper* m_pPipeline{};

		bool m_ShouldUpdateDescriptorSets{true};
	};
}
#endif // !MaterialIncluded