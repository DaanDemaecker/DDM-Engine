// TextureDescriptorObject.cpp

//File includes
#include "TextureDescriptorObject.h"
#include "../../Vulkan/VulkanRenderer.h"

DDM3::TextureDescriptorObject::TextureDescriptorObject()
	:DescriptorObject(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
{
	auto& renderer{ VulkanRenderer::GetInstance() };
	m_PlaceholderImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	m_PlaceholderImageInfo.imageView = renderer.GetDefaultImageView();
	m_PlaceholderImageInfo.sampler = renderer.GetSampler();
}

void DDM3::TextureDescriptorObject::AddTextures(Texture& texture)
{
	// Add the texture to the list of textures
	m_Textures.push_back(texture);

	SetupImageInfos();
}

void DDM3::TextureDescriptorObject::AddTextures(std::initializer_list<const std::string>& filePaths)
{
	// Initialize index variable
	int index{static_cast<int>(m_Textures.size()) - 1};

	if (index < 0)
	{
		index = 0;
	}

	// Resize textures to textureAmount
	m_Textures.resize(filePaths.size() + m_Textures.size());

	auto& renderer{ VulkanRenderer::GetInstance() };

	// Loop trough all filePaths
	for (const auto& path : filePaths)
	{
		// Create texture
		renderer.CreateTexture(m_Textures[index], path);

		// Increment index
		++index;
	}

	SetupImageInfos();
}

void DDM3::TextureDescriptorObject::AddTexture(const std::string& filePath)
{
	auto& renderer{ VulkanRenderer::GetInstance() };

	m_Textures.resize(m_Textures.size() + 1);

	renderer.CreateTexture(m_Textures[m_Textures.size() - 1], filePath);

	SetupImageInfos();
}


void DDM3::TextureDescriptorObject::Clear()
{
	m_Textures.clear();
	
	SetupImageInfos();
}

int DDM3::TextureDescriptorObject::GetTextureAmount() const
{
	return m_Textures.size();
}

DDM3::TextureDescriptorObject::~TextureDescriptorObject()
{
	// Get the device and clean up all the textures

	auto device{ VulkanRenderer::GetInstance().GetDevice() };

	for (auto& texture : m_Textures)
	{
		texture.Cleanup(device);
	}
}

void DDM3::TextureDescriptorObject::AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int amount, int /*index*/)
{
	for (int i{}; i < amount; i++)
	{
		VkWriteDescriptorSet descriptorWrite{};

		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

		// Set all fields of current binding with correct values
		descriptorWrite.dstBinding = binding;
		descriptorWrite.dstArrayElement = i;
		descriptorWrite.descriptorType = m_Type;
		descriptorWrite.descriptorCount = 1;

		if (m_ImageInfos.size() >= i + 1)
		{
			descriptorWrite.pImageInfo = &m_ImageInfos[i];
		}
		else
		{
			descriptorWrite.pImageInfo = &m_PlaceholderImageInfo;
		}
		descriptorWrite.dstSet = descriptorSet;

		descriptorWrites.push_back(descriptorWrite);
	}

	binding++;
}

void DDM3::TextureDescriptorObject::SetupImageInfos()
{
	m_ImageInfos.clear();
	// resize image infos
	m_ImageInfos.resize(m_Textures.size());

	// Get the sampler
	auto& sampler{ VulkanRenderer::GetInstance().GetSampler() };

	int index{};

	// Loop trough all the textures
	for (auto& texture : m_Textures)
	{
		// Set image layout to shader read optimal
		m_ImageInfos[index].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		// Set correct image view
		m_ImageInfos[index].imageView = texture.imageView;
		// Set sampler
		m_ImageInfos[index].sampler = sampler;

		index++;
	}
}
