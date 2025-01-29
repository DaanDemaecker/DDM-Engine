// CubeMapMaterial.cpp

// File includes
#include "CubeMapMaterial.h"
#include "../../Vulkan/VulkanRenderer.h"
#include "../../Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "../../DataTypes/DescriptorObjects/TextureDescriptorObject.h"
#include "../../Managers/ConfigManager.h"

// Standard library includes
#include <stdexcept>

DDM3::CubeMapMaterial::CubeMapMaterial(const std::initializer_list<const std::string>&& filePaths)
	:CubeMapMaterial(filePaths)
{

}

DDM3::CubeMapMaterial::CubeMapMaterial(const std::initializer_list<const std::string>& filePaths)
	:Material(ConfigManager::GetInstance().GetString("SkyboxPipelineName"))
{
	Texture cubeTexture{};

	// Create the cube texture
	VulkanRenderer::GetInstance().CreateCubeTexture(cubeTexture, filePaths);

	// Create the descriptor object and give the cube texture by value
	m_pDescriptorObject = std::make_unique<TextureDescriptorObject>();
	m_pDescriptorObject->AddTextures(cubeTexture);
}

void DDM3::CubeMapMaterial::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
{
	// Get pointer to the descriptorpool
	auto descriptorPool = GetDescriptorPool();

	// Create a vector for the descriptor objects
	std::vector<DescriptorObject*> descriptorObjectList{};

	// Add all the descriptor objects of the model to the list
	for (auto& descriptorObject : descriptorObjects)
	{
		descriptorObjectList.push_back(descriptorObject);
	}

	// Add the descriptor object holding the texture
	descriptorObjectList.push_back(m_pDescriptorObject.get());

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);
}
