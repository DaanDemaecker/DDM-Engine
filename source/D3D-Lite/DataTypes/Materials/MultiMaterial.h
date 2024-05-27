#ifndef MultiMaterialIncluded
#define MultiMaterialIncluded

// File includes
#include "Material.h"
#include "../DescriptorObjects/UboDescriptorObject.h"
#include "../DescriptorObjects/TextureDescriptorObject.h"


// Standard library includes
#include <vector>

namespace D3D
{
	struct MultiShaderBuffer
	{
		alignas(4) int diffuseAmount{0};
		alignas(4) uint32_t diffuseEnabled{0};
		alignas(4) int normalAmount{ 0 };
		alignas(4) uint32_t normalEnabled { 0 };
		alignas(4) int glossAmount{ 0 };
		alignas(4) uint32_t glossEnabled { 0 };
		alignas(4) int specularAmount{ 0 };
		alignas(4) uint32_t specularEnabled { 0 };
	};
	
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

		void AddNormalMap(std::initializer_list<const std::string>&& filePaths);

		void AddNormalMap(std::initializer_list<const std::string>& filePaths);

		void AddGlossTextures(std::initializer_list<const std::string>&& filePaths);

		void AddGlossTextures(std::initializer_list<const std::string>& filePaths);

		void AddSpecularTextures(std::initializer_list<const std::string>&& filePaths);

		void AddSpecularTextures(std::initializer_list<const std::string>& filePaths);


	private:
		const int m_TextLength{ 125 };
		struct GuiObject
		{
			char diffuseName[125]{};
			char normalMapName[125]{};
			char glossName[125]{};
			char specularName[125]{};
		} m_GuiObject;


		MultiShaderBuffer m_MultiShaderBuffer{};
		std::unique_ptr<D3D::UboDescriptorObject<MultiShaderBuffer>> m_pMultiShaderBufferDescriptor{};

		std::unique_ptr<D3D::TextureDescriptorObject> m_pDiffuseTextureObject{};

		std::unique_ptr<D3D::TextureDescriptorObject> m_pNormalTextureObject{};

		std::unique_ptr<D3D::TextureDescriptorObject> m_pGlossTextureObject{};

		std::unique_ptr<D3D::TextureDescriptorObject> m_pSpecularTextureObject{};

		void UpdateShaderBuffer();
	};
}


#endif // !MultiMaterialIncluded
