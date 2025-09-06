// TextureDescriptorObject.cpp

//File includes
#include "TextureDescriptorObject.h"
#include "Vulkan/VulkanObject.h"

DDM::TextureDescriptorObject::TextureDescriptorObject()
	// Type of this object is combined image sampler
	:DescriptorObject(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
{
	// Set up placeholder image info
	auto& vulkanObject{ VulkanObject::GetInstance() };
	m_PlaceholderImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	m_PlaceholderImageInfo.imageView = vulkanObject.GetDefaultImageView();
	m_PlaceholderImageInfo.sampler = vulkanObject.GetSampler();
}

DDM::TextureDescriptorObject::~TextureDescriptorObject()
{
	
}

void DDM::TextureDescriptorObject::AddTexture(std::shared_ptr<Image> texture)
{
	// Add the texture to the list of textures
	m_Textures.push_back(texture);

	// Indicate that image infos should be set up
	m_AreImageInfosSetup = false;
}

void DDM::TextureDescriptorObject::AddTextures(std::initializer_list<const std::string>& filePaths)
{
	// Loop through all filepaths and add them to the list
	for (const auto& filePath : filePaths)
	{
		AddTexture(filePath);
	}
}

void DDM::TextureDescriptorObject::AddTexture(const std::string& filePath)
{
	// Create new texture
	m_Textures.push_back(std::make_shared<Image>(filePath));

	// Indicate that image infos should be set up
	m_AreImageInfosSetup = false;
}


void DDM::TextureDescriptorObject::Clear()
{
	// Clear list
	m_Textures.clear();
	
	// Indicate that image infos should be set up
	m_AreImageInfosSetup = false;
}

size_t DDM::TextureDescriptorObject::GetTextureAmount() const
{
	// Return size of textures
	return m_Textures.size();
}

void DDM::TextureDescriptorObject::AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int amount, int /*index*/)
{
	// If image infos need to be set up, do so
	if (!m_AreImageInfosSetup)
	{
		SetupImageInfos();
	}

	// Loop through the amount of textures in the array
	for (int i{}; i < amount; i++)
	{
		VkWriteDescriptorSet descriptorWrite{};

		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

		// Set all fields of current binding with correct values
		descriptorWrite.dstBinding = binding;
		descriptorWrite.dstArrayElement = i;
		descriptorWrite.descriptorType = m_Type;
		descriptorWrite.descriptorCount = 1;

		// If one is available, set pImageInfo to the correct one from the array
		if (m_ImageInfos.size() >= i + 1)
		{
			descriptorWrite.pImageInfo = &m_ImageInfos[i];
		}
		// If none are available, set pImageInfo to placeholder
		else
		{
			descriptorWrite.pImageInfo = &m_PlaceholderImageInfo;
		}

		// Set destination descriptor set to the one given
		descriptorWrite.dstSet = descriptorSet;

		// Add to the list
		descriptorWrites.push_back(descriptorWrite);
	}

	// Increase binding
	binding++;
}

void DDM::TextureDescriptorObject::SetupImageInfos()
{
	// Clear list
	m_ImageInfos.clear();

	// resize image infos
	m_ImageInfos.resize(m_Textures.size());

	// Loop trough all the textures
	for (int i{}; i < m_Textures.size(); ++i)
	{
		// Set image layout to shader read optimal
		m_ImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		// Set correct image view
		m_ImageInfos[i].imageView = m_Textures[i]->GetImageView();
		// Set sampler
		m_ImageInfos[i].sampler = m_PlaceholderImageInfo.sampler;
	}

	// Indicate that image infos are set up
	m_AreImageInfosSetup = true;
}
