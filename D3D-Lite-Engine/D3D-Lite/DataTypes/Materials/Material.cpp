#include "Material.h"
#include "../../Vulkan/VulkanRenderer.h"
#include "../../Utils/Utils.h"
#include "../../Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "../../Includes/STBIncludes.h"
#include "../../Vulkan/VulkanWrappers/PipelineWrapper.h"

DDM3::Material::Material(const std::string& pipelineName)
{
	// Get the requested pipeline from the renderer
	m_pPipeline = VulkanRenderer::GetInstance().GetPipeline(pipelineName); 
}

DDM3::PipelineWrapper* DDM3::Material::GetPipeline()
{
	return m_pPipeline;
}

void DDM3::Material::CreateDescriptorSets(MeshRenderComponent* pModel, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = GetDescriptorPool();
	// Add model to descriptorpool wrapper
	descriptorPool->AddModel(pModel);
	// Create descriptorpools
	descriptorPool->CreateDescriptorSets(GetDescriptorLayout(), descriptorSets);
}

void DDM3::Material::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
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
	descriptorObjectList.push_back(VulkanRenderer::GetInstance().GetLightDescriptor());

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);

	m_ShouldUpdateDescriptorSets = false;
}

VkDescriptorSetLayout DDM3::Material::GetDescriptorLayout()
{
	return m_pPipeline->GetDescriptorSetLayout();
}

DDM3::DescriptorPoolWrapper* DDM3::Material::GetDescriptorPool()
{
	return m_pPipeline->GetDescriptorPool();
}
