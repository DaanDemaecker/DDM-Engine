// MultiMaterial.h
// This material has a variable amount of diffuse, normal, gloss and specular textures


#ifndef _MULTI_MATERIAL_
#define _MULTI_MATERIAL_

// Parent include
#include "Material.h"

// File includes
#include "DataTypes/DescriptorObjects/UboDescriptorObject.h"
#include "DataTypes/DescriptorObjects/TextureDescriptorObject.h"

// Standard library includes
#include <vector>

namespace DDM
{
	enum
	{
		mm_Diffuse = 0,
		mm_Normal = 1,
		mm_Gloss = 2,
		mm_Specular = 3,
		mm_Last = mm_Specular
	};


	// This object holds the info the shader will need to know what images are in use
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
		/// <summary>
		/// Constructor
		/// </summary>
		MultiMaterial();

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~MultiMaterial() override;

		/// <summary>
		/// Renders GUI objects
		/// </summary>
		virtual void OnGUI() override;

		// Update the descriptorsets
		// Parameters:
		//     descriptorsets: the descriptorsets that should be updated
		//     descriptorObjects: a vector of pointers to descriptorobjects in the same order as the shader code
		virtual void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects) override;

		/// <summary>
		/// Add a single diffuse texture
		/// </summary>
		/// <param name="filePath: ">Path to the requested image</param>
		void AddDiffuseTexture(std::string& filePath);

		/// <summary>
		/// Add a single diffuse texture
		/// </summary>
		/// <param name="filePath: ">Path to the requested image</param>
		void AddDiffuseTexture(std::string&& filePath);

		/// <summary>
		/// Add a single normal texture
		/// </summary>
		/// <param name="filePath: ">Path to the requested image</param>
		void AddNormalMap(std::string& filePath);

		/// <summary>
		/// Add a single normal texture
		/// </summary>
		/// <param name="filePath: ">Path to the requested image</param>
		void AddNormalMap(std::string&& filePath);

		/// <summary>
		/// Add a single gloss texture
		/// </summary>
		/// <param name="filePath: ">Path to the requested image</param>
		void AddGlossTexture(std::string& filePath);

		/// <summary>
		/// Add a single gloss texture
		/// </summary>
		/// <param name="filePath: ">Path to the requested image</param>
		void AddGlossTexture(std::string&& filePath);

		/// <summary>
		/// Add a single specular texture
		/// </summary>
		/// <param name="filePath: ">Path to the requested image</param>
		void AddSpecularTexture(std::string& filePath);

		/// <summary>
		/// Add a single specular texture
		/// </summary>
		/// <param name="filePath: ">Path to the requested image</param>
		void AddSpecularTexture(std::string&& filePath);


	private:
		// The max length of the input string for ImGui
		const int m_TextLength{ 125 };
		
		// This struct will hold the info used for rendering the ImGui and retrieving info from it
		struct GuiObject
		{
			bool diffuseHovered{ false };
			char diffuseName[125]{};
			bool normalMapHovered{ false };
			char normalMapName[125]{};
			bool glossHovered{ false };
			char glossName[125]{};
			bool specularHovered{false};
			char specularName[125]{};
		} m_GuiObject;

		// Holds info for the shader
		MultiShaderBuffer m_MultiShaderBuffer{};

		// Descriptor object for the MultiShaderBuffer object
		std::unique_ptr<DDM::UboDescriptorObject<MultiShaderBuffer>> m_pMultiShaderBufferDescriptor{};

		// Map of texture descriptor objects
		std::vector<std::unique_ptr<DDM::TextureDescriptorObject>> m_pTextureObjects{};

		/// <summary>
		/// Update the MultiShaderBuffer object
		/// </summary>
		void UpdateShaderBuffer();

		/// <summary>
		/// Callback function for when a file is dropped inside the window
		/// </summary>
		/// <param name="count: ">Amount of files dropped</param>
		/// <param name="paths: ">List of strings that hold the filepath to the dropped files</param>
		void DropFileCallback(int count, const char** paths);

		/// <summary>
		/// Override a filename string from the ImGui
		/// </summary>
		/// <param name="text ">pointer to the beginning of the string</param>
		/// <param name="textLength: ">Length of text</param>
		/// <param name="path: ">Name of the filepath given by callback</param>
		void SetFileName(char* text, int textLength, const char* path);

		/// <summary>
		/// Draw ImGui for the diffuse texture
		/// </summary>
		void DiffuseGui();

		/// <summary>
		/// Draw ImGui for the normal texture
		/// </summary>
		void NormalMapGui();

		/// <summary>
		/// Draw ImGui for the gloss texture
		/// </summary>
		void GlossMapGui();

		/// <summary>
		/// Draw ImGui for the specular texture
		/// </summary>
		void SpecularMapGui();
	};
}


#endif // !_MULTI_MATERIAL_
