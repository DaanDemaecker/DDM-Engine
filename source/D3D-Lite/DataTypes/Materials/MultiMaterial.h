#ifndef MultiMaterialIncluded
#define MultiMaterialIncluded

// File includes
#include "Material.h"
#include "../DescriptorObjects/UboDescriptorObject.h"


// Standard library includes
#include <vector>

namespace D3D
{
	struct MultiShaderBuffer
	{
		int diffuseAmount{0};
		bool diffuseEnabled{false};
	};

	class TextureDescriptorObject;
	
	class MultiMaterial final : public Material
	{
	public:
		MultiMaterial() = delete;

		MultiMaterial(const std::string& pipeline = "Default");

		virtual ~MultiMaterial() override = default;

		// Update the descriptorsets
		// Parameters:
		//     descriptorsets: the descriptorsets that should be updated
		//     descriptorObjects: a vector of pointers to descriptorobjects in the same order as the shader code
		virtual void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects);

	private:
		MultiShaderBuffer m_MultiShaderBuffer{};
		std::unique_ptr<D3D::UboDescriptorObject<MultiShaderBuffer>> m_pMultiShaderBufferDescriptor{};


		std::unique_ptr<D3D::TextureDescriptorObject> m_pDiffuseTextureObject{};
	};
}


#endif // !MultiMaterialIncluded
