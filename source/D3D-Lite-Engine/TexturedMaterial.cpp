#include "TexturedMaterial.h"
#include "VulkanRenderer.h"
#include "Utils.h"
#include "DescriptorPoolWrapper.h"
#include "ModelComponent.h"
#include "PipelineWrapper.h"

#include "STBIncludes.h"

D3D::TexturedMaterial::TexturedMaterial(std::initializer_list<const std::string>&& filePaths, const std::string& pipelineName)
	:Material(pipelineName)
{
	// Create a descriptor object with the list of file paths given
	m_pDescriptorObject = std::make_unique<D3D::TextureDescriptorObject>(filePaths);

	// Create sampler
	CreateTextureSampler();
}

void D3D::TexturedMaterial::CreateDescriptorSets(ModelComponent* pModel, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Get descriptorpool associated with this material
	auto descriptorPool = GetDescriptorPool();
	// Add model to descriptorpool
	descriptorPool->AddModel(pModel);
	// Create descriptorpool
	descriptorPool->CreateDescriptorSets(GetDescriptorLayout(), descriptorSets);
}

void D3D::TexturedMaterial::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
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

	// Add the descriptor object holding the textures
	descriptorObjectList.push_back(m_pDescriptorObject.get());

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);
}

VkDescriptorSetLayout D3D::TexturedMaterial::GetDescriptorLayout()
{
	return m_pPipeline->GetDescriptorSetLayout();
}

D3D::DescriptorPoolWrapper* D3D::TexturedMaterial::GetDescriptorPool()
{
	return m_pPipeline->GetDescriptorPool();
}

void D3D::TexturedMaterial::CreateTextureSampler()
{
	m_TextureSampler = VulkanRenderer::GetInstance().GetSampler();
}
