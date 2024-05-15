#include "TexturedMaterial.h"
#include "../../Vulkan/VulkanRenderer.h"
#include "../../Utils/Utils.h"
#include "../../Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "../../Components/MeshRenderComponent.h"
#include "../../Vulkan/VulkanWrappers/PipelineWrapper.h"

#include "../../Includes/STBIncludes.h"

D3D::TexturedMaterial::TexturedMaterial(std::initializer_list<const std::string>&& filePaths, const std::string& pipelineName)
	:Material(pipelineName)
{
	for (auto& filePath : filePaths)
	{
		// Create a descriptor object with the list of file paths given
		m_pDescriptorObjects.push_back(std::make_unique<D3D::TextureDescriptorObject>(filePath));
	}
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

	for (auto& descriptorObject : m_pDescriptorObjects)
	{
		// Add the descriptor object holding the textures
		descriptorObjectList.push_back(descriptorObject.get());
	}	

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);
}