// TexturedMaterial.h
// This material holds a number of textures and sends them to the shader

#ifndef _TEXTURED_MATERIAL_
#define _TEXTURED_MATERIAL_

// Parent include
#include "Material.h"

// File includes
#include "DataTypes/DescriptorObjects/TextureDescriptorObject.h"

// Standard library includes
#include <initializer_list>
#include <iostream>

namespace DDM
{
	class TexturedMaterial final : public Material
	{
	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pipelineName: ">Name of the graphics pipeline this material will use</param>
		TexturedMaterial(const std::string& pipelineName);

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pipelineName: ">Name of the graphics pipeline this material will use</param>
		TexturedMaterial(const std::string&& pipelineName = "Default");

		/// <summary>
		/// Default destructor
		/// </summary>
		~TexturedMaterial() = default;

		/// <summary>
		/// Adds a single texture to the list
		/// </summary>
		/// <param name="path: ">The path to the texture file</param>
		void AddTexture(const std::string& path);

		/// <summary>
		/// Adds a single texture to the list
		/// </summary>
		/// <param name="path: ">The path to the texture file</param>
		void AddTexture(const std::string&& path);

		/// <summary>
		/// Update the descriptorsets
		/// </summary>
		/// <param name="descriptorSets: ">tDescriptorsets that should be updated</param>
		/// <param name="descriptorObjects: ">Vector of pointers to descriptorobjects in the same order as the shader code</param>
		virtual void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects) override;

	private:
		// List of descriptorobjects that hold the textures
		std::vector<std::unique_ptr<TextureDescriptorObject>> m_pDescriptorObjects{};
	};
}
#endif // !_TEXTURED_MATERIAL_