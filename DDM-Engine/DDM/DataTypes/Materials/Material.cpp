// Material.cpp

// Header include
#include "Material.h"

// File includes
#include "Includes/STBIncludes.h"

#include "Utils/Utils.h"

#include "Vulkan/VulkanObject.h"
#include "Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "Vulkan/VulkanWrappers/PipelineWrapper.h"

DDM::Material::Material(const std::string& pipelineName)
{
	// Get the requested pipeline from the renderer
	m_pPipeline = VulkanObject::GetInstance().GetPipeline(pipelineName);
}

DDM::Material& DDM::Material::operator=(DDM::Material&& other) noexcept
{
	// Copy over the pipeline
	m_pPipeline = other.m_pPipeline;

	// Copy over wether descriptor sets should be updated 
	m_ShouldUpdateDescriptorSets = other.m_ShouldUpdateDescriptorSets;

	return *this;
}

DDM::PipelineWrapper* DDM::Material::GetPipeline()
{
	// Return pointer to the pipeline
	return m_pPipeline;
}

void DDM::Material::CreateDescriptorSets(MeshRenderComponent* pModel, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = GetDescriptorPool();

	// Add model to descriptorpool wrapper
	descriptorPool->AddModel(pModel);

	// Create descriptorsets
	descriptorPool->CreateDescriptorSets(GetDescriptorLayout(), descriptorSets);
}

void DDM::Material::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = GetDescriptorPool();

	// Create list of descriptor objects and add the objects of the model to it
	std::vector<DescriptorObject*> descriptorObjectList{};

	for (auto& descriptorObject : descriptorObjects)
	{
		descriptorObjectList.push_back(descriptorObject);
	}

	// Add the descriptor of the global light object
	descriptorObjectList.push_back(VulkanObject::GetInstance().GetLightDescriptor());

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);

	// Indicate that descriptor sets are updated
	m_ShouldUpdateDescriptorSets = false;
}

VkDescriptorSetLayout DDM::Material::GetDescriptorLayout()
{
	// Return descriptorset layout
	return m_pPipeline->GetDescriptorSetLayout();
}

DDM::DescriptorPoolWrapper* DDM::Material::GetDescriptorPool()
{
	// Return descriptorpool
	return m_pPipeline->GetDescriptorPool();
}
