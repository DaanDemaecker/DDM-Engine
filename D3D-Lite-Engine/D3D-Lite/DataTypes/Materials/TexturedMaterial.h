#ifndef TexturedMaterialIncluded
#define TexturedMaterialIncluded

#include "Material.h"
#include "../DescriptorObjects/TextureDescriptorObject.h"

#include <initializer_list>
#include <iostream>

namespace DDM3
{
	class TexturedMaterial final : public Material
	{
	public:
		TexturedMaterial(std::initializer_list<const std::string>&& filePaths, const std::string& pipelineName = "Default");
		~TexturedMaterial() = default;

		// Update the descriptorsets
		// Parameters:
		//     descriptorsets: the descriptorsets that should be updated
		//     descriptorObjects: a vector of pointers to descriptorobjects in the same order as the shader code
		virtual void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects) override;

	private:
		std::vector<std::unique_ptr<TextureDescriptorObject>> m_pDescriptorObjects{};
	};
}
#endif // !TexturedMaterialIncluded