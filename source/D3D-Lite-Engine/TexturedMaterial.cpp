#include "TexturedMaterial.h"
#include "VulkanRenderer.h"
#include "Utils.h"
#include "DescriptorPoolManager.h"
#include "DescriptorPoolWrapper.h"
#include "ModelComponent.h"

#include "STBIncludes.h"

D3D::TexturedMaterial::TexturedMaterial(std::initializer_list<const std::string> filePaths, const std::string& pipelineName)
	:Material(pipelineName)
{
	m_TextureAmount = static_cast<int>(filePaths.size());

	m_Textures.resize(m_TextureAmount);

	int index{};

	auto& renderer{ VulkanRenderer::GetInstance() };

	for (const auto& path : filePaths)
	{
		renderer.CreateTexture(m_Textures[index], path);
		++index;
	}

	CreateTextureSampler();
}

D3D::TexturedMaterial::~TexturedMaterial()
{
	auto device{ VulkanRenderer::GetInstance().GetDevice() };

	for (auto& texture : m_Textures)
	{
		texture.Cleanup(device);
	}
}

void D3D::TexturedMaterial::CreateDescriptorSets(ModelComponent* pModel, std::vector<VkDescriptorSet>& descriptorSets)
{
	auto descriptorPool = GetDescriptorPool();
	descriptorPool->AddModel(pModel);
	descriptorPool->CreateDescriptorSets(*GetDescriptorLayout(), descriptorSets);
}

void D3D::TexturedMaterial::UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets)
{
	auto descriptorPool = GetDescriptorPool();
	std::vector<std::vector<VkBuffer>> uboBuffferList{ uboBuffers, D3D::VulkanRenderer::GetInstance().GetLightBuffers() };

	std::vector<VkDeviceSize> uboSizes(2);
	uboSizes[0] = sizeof(UniformBufferObject);
	uboSizes[1] = sizeof(DirectionalLightStruct);

	std::vector<VkImageView> imageViews{};
	for (auto& texture : m_Textures)
	{
		imageViews.push_back(texture.imageView);
	}

	descriptorPool->UpdateDescriptorSets(uboBuffferList, uboSizes, descriptorSets, imageViews);
}

VkDescriptorSetLayout* D3D::TexturedMaterial::GetDescriptorLayout()
{
	return D3D::VulkanRenderer::GetInstance().GetDescriptorSetLayout(1, 1, m_TextureAmount);
}

D3D::DescriptorPoolWrapper* D3D::TexturedMaterial::GetDescriptorPool()
{
	return D3D::VulkanRenderer::GetInstance().GetDescriptorPoolManager()->GetDescriptorPool(2, m_TextureAmount);
}

void D3D::TexturedMaterial::CreateTextureSampler()
{
	m_TextureSampler = VulkanRenderer::GetInstance().GetSampler();
}
