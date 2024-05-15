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
		alignas(4) int diffuseAmount{0};
		alignas(4) uint32_t diffuseEnabled{0};
	};

	class TextureDescriptorObject;
	
	class MultiMaterial final : public Material
	{
	public:
		MultiMaterial() = delete;

		MultiMaterial(const std::string& pipeline = "Default");

		virtual ~MultiMaterial() override = default;

		virtual void OnGUI() override;

		// Update the descriptorsets
		// Parameters:
		//     descriptorsets: the descriptorsets that should be updated
		//     descriptorObjects: a vector of pointers to descriptorobjects in the same order as the shader code
		virtual void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects);

		void AddDiffuseTextures(std::initializer_list<const std::string>&& filePaths);

		void AddDiffuseTextures(std::initializer_list<const std::string>& filePaths);


	private:
		MultiShaderBuffer m_MultiShaderBuffer{};
		std::unique_ptr<D3D::UboDescriptorObject<MultiShaderBuffer>> m_pMultiShaderBufferDescriptor{};

		std::unique_ptr<D3D::TextureDescriptorObject> m_pDiffuseTextureObject{};

		bool m_ShowGuiWindow{ true };

		void UpdateShaderBuffer();
	};
}


#endif // !MultiMaterialIncluded
