#ifndef TexturedMaterialIncluded
#define TexturedMaterialIncluded

#include "Material.h"
#include "TextureDescriptorObject.h"

#include <initializer_list>
#include <iostream>

namespace D3D
{
	class TexturedMaterial final : public Material
	{
	public:
		TexturedMaterial(std::initializer_list<const std::string>&& filePaths, const std::string& pipelineName = "Default");
		~TexturedMaterial() = default;

		// Create the descriptorsets
		// Parameters:
		//     pModel: the model that the descriptorsets belong to
		//     descriptorSets: vector of descriptorsets that have to be created
		virtual void CreateDescriptorSets(MeshRenderComponent* pModel, std::vector<VkDescriptorSet>& descriptorSets) override;

		// Update the descriptorsets
		// Parameters:
		//     descriptorsets: the descriptorsets that should be updated
		//     descriptorObjects: a vector of pointers to descriptorobjects in the same order as the shader code
		virtual void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects) override;


		virtual VkDescriptorSetLayout GetDescriptorLayout() override;

		virtual DescriptorPoolWrapper* GetDescriptorPool() override;

	private:
		std::unique_ptr<TextureDescriptorObject> m_pDescriptorObject{};

		// The sampler for the textures
		VkSampler m_TextureSampler{};

		//Initialization functions
		void CreateTextureSampler();
	};
}
#endif // !TexturedMaterialIncluded