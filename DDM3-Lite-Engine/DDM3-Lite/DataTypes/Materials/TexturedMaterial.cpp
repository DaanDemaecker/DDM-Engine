#include "TexturedMaterial.h"
#include "../../Vulkan/VulkanRenderer.h"
#include "../../Utils/Utils.h"
#include "../../Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "../../Components/MeshRenderComponent.h"
#include "../../Vulkan/VulkanWrappers/PipelineWrapper.h"

#include "../../Includes/STBIncludes.h"

DDM3::TexturedMaterial::TexturedMaterial(std::initializer_list<const std::string>&& filePaths, const std::string& pipelineName)
	:Material(pipelineName)
{
	m_pDescriptorObjects.resize(filePaths.size());
	
	int index = 0;
	for (const auto& filePath : filePaths)
	{
		m_pDescriptorObjects[index] = std::make_unique<DDM3::TextureDescriptorObject>();
		m_pDescriptorObjects[index]->AddTexture(filePath);
		++index;
	}

	m_ShouldUpdateDescriptorSets = true;
}

DDM3::TexturedMaterial::TexturedMaterial(const std::string& pipelineName)
	:Material(pipelineName)
{
}

DDM3::TexturedMaterial::TexturedMaterial(const std::string&& pipelineName)
	:TexturedMaterial(pipelineName)
{
}


void DDM3::TexturedMaterial::AddTexture(std::string& path)
{
	auto descriptorObject = std::make_unique<DDM3::TextureDescriptorObject>();
	descriptorObject->AddTexture(path);
	m_pDescriptorObjects.push_back(std::move(descriptorObject));

	m_ShouldUpdateDescriptorSets = true;
}

void DDM3::TexturedMaterial::AddTexture(std::string&& path)
{
	// Pass function on
	AddTexture(path);
}

void DDM3::TexturedMaterial::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
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
	descriptorObjectList.push_back(VulkanRenderer::GetInstance().GetLightDescriptor());

	for (auto& descriptorObject : m_pDescriptorObjects)
	{
		// Add the descriptor object holding the textures
		descriptorObjectList.push_back(descriptorObject.get());
	}	

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);

	m_ShouldUpdateDescriptorSets = false;
}