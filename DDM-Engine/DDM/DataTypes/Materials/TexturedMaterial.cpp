// TexturedMaterial.cpp

// Header include
#include "TexturedMaterial.h"

// File includes
#include "Includes/STBIncludes.h"

#include "Vulkan/VulkanObject.h"
#include "Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "Vulkan/VulkanWrappers/PipelineWrapper.h"

#include "Utils/Utils.h"

#include "Components/MeshRenderer.h"


DDM::TexturedMaterial::TexturedMaterial(const std::string& pipelineName)
	// Propagate to parent class
	:Material(pipelineName)
{
}

DDM::TexturedMaterial::TexturedMaterial(const std::string&& pipelineName)
	// Propagate to parent class
	:Material(pipelineName)
{
}


void DDM::TexturedMaterial::AddTexture(const std::string& path)
{
	// Create a descriptorobject and add the texture
	auto descriptorObject = std::make_unique<DDM::TextureDescriptorObject>();
	descriptorObject->AddTexture(path);

	// Add object to list
	m_pDescriptorObjects.push_back(std::move(descriptorObject));

	// Indicate that descriptorsets should be updated
	m_ShouldUpdateDescriptorSets = true;
}

void DDM::TexturedMaterial::AddTexture(const std::string&& path)
{
	// Propagate to lvalue overloaded function
	AddTexture(path);
}

void DDM::TexturedMaterial::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = GetDescriptorPool();

	// Create list of descriptor objects and add the objects of the model to it
	std::vector<DescriptorObject*> descriptorObjectList{};

	for (auto& descriptorObject : descriptorObjects)
	{
		descriptorObjectList.push_back(descriptorObject);
	}

	// Add the descriptor object of the global light
	descriptorObjectList.push_back(VulkanObject::GetInstance().GetLightDescriptor());

	for (auto& descriptorObject : m_pDescriptorObjects)
	{
		// Add the descriptor object holding the textures
		descriptorObjectList.push_back(descriptorObject.get());
	}	

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);

	// Indicate that descriptorsets were updated
	m_ShouldUpdateDescriptorSets = false;
}