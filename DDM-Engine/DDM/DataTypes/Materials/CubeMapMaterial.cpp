// CubeMapMaterial.cpp

// File includes
#include "CubeMapMaterial.h"
#include "Vulkan/VulkanObject.h"
#include "../../Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "../../DataTypes/DescriptorObjects/TextureDescriptorObject.h"
#include "../../Managers/ConfigManager.h"

// Standard library includes
#include <stdexcept>

DDM::CubeMapMaterial::CubeMapMaterial()
	:CubeMapMaterial(ConfigManager::GetInstance().GetString("SkyboxPipelineName"))
{
}

DDM::CubeMapMaterial::CubeMapMaterial(std::string&& pipelineName)
	:CubeMapMaterial(pipelineName)
{
}

DDM::CubeMapMaterial::CubeMapMaterial(std::string& pipelineName)
	:Material(pipelineName)
{
	// Create the descriptor object and give the cube texture by value
	m_pDescriptorObject = std::make_unique<TextureDescriptorObject>();

	m_FilePaths.resize(6);
}

void DDM::CubeMapMaterial::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
{
	if (!m_Initialized)
	{
		SetupCubeTexture();
	}

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

void DDM::CubeMapMaterial::SetTextureName(const std::string& name, int index)
{
	m_FilePaths[index] = name;
}

void DDM::CubeMapMaterial::SetupCubeTexture()
{
	std::shared_ptr<Image> cubeTexture{ std::make_shared<Image>() };

	// Create the cube texture
	VulkanObject::GetInstance().CreateCubeTexture(cubeTexture.get(), m_FilePaths);

	// Add texture to descriptor object
	m_pDescriptorObject->AddTexture(cubeTexture);

	m_Initialized = true;
}

void DDM::CubeMapMaterial::SetRight(const std::string& filepath)
{
	// Propagate to SetTextureName function
	SetTextureName(filepath, sideRight);
}

void DDM::CubeMapMaterial::SetRight(const std::string&& filepath)
{
	// Propagate to SetTextureName function
	SetTextureName(filepath, sideRight);
}

void DDM::CubeMapMaterial::SetLeft(const std::string& filepath)
{
	// Propagate to SetTextureName function
	SetTextureName(filepath, sideLeft);
}

void DDM::CubeMapMaterial::SetLeft(const std::string&& filepath)
{
	// Propagate to SetTextureName function
	SetTextureName(filepath, sideLeft);
}

void DDM::CubeMapMaterial::SetUp(const std::string& filepath)
{
	// Propagate to SetTextureName function
	SetTextureName(filepath, sideUp);
}

void DDM::CubeMapMaterial::SetUp(const std::string&& filepath)
{
	// Propagate to SetTextureName function
	SetTextureName(filepath, sideUp);
}

void DDM::CubeMapMaterial::SetDown(const std::string& filepath)
{
	// Propagate to SetTextureName function
	SetTextureName(filepath, sideDown);
}

void DDM::CubeMapMaterial::SetDown(const std::string&& filepath)
{
	// Propagate to SetTextureName function
	SetTextureName(filepath, sideDown);
}

void DDM::CubeMapMaterial::SetFront(const std::string& filepath)
{
	// Propagate to SetTextureName function
	SetTextureName(filepath, sideFront);
}

void DDM::CubeMapMaterial::SetFront(const std::string&& filepath)
{
	// Propagate to SetTextureName function
	SetTextureName(filepath, sideFront);
}

void DDM::CubeMapMaterial::SetBack(const std::string& filepath)
{
	// Propagate to SetTextureName function
	SetTextureName(filepath, sideBack);
}

void DDM::CubeMapMaterial::SetBack(const std::string&& filepath)
{
	// Propagate to SetTextureName function
	SetTextureName(filepath, sideBack);
}
