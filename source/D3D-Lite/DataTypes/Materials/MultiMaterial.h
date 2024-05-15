#ifndef MultiMaterialIncluded
#define MultiMaterialIncluded

// File includes
#include "Material.h"


namespace D3D
{
	class MultiMaterial final : public Material
	{
	public:
		MultiMaterial() = delete;

		MultiMaterial(const std::string& pipeline = "Default");

		virtual ~MultiMaterial() = default;

		// Create the descriptorsets
		// Parameters:
		//     pModel: the model that the descriptorsets belong to
		//     descriptorSets: vector of descriptorsets that have to be created
		virtual void CreateDescriptorSets(MeshRenderComponent* pModel, std::vector<VkDescriptorSet>& descriptorSets);

		// Update the descriptorsets
		// Parameters:
		//     descriptorsets: the descriptorsets that should be updated
		//     descriptorObjects: a vector of pointers to descriptorobjects in the same order as the shader code
		virtual void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects);

	private:

	};
}


#endif // !MultiMaterialIncluded
