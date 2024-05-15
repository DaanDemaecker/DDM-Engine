
#include "MultiMaterial.h"

D3D::MultiMaterial::MultiMaterial(const std::string& pipeline)
	:Material(pipeline)
{

}

void D3D::MultiMaterial::CreateDescriptorSets(MeshRenderComponent* pModel, std::vector<VkDescriptorSet>& descriptorSets)
{
}

void D3D::MultiMaterial::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
{
}
