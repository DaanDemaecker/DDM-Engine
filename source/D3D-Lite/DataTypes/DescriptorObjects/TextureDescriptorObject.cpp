// TextureDescriptorObject.cpp

//File includes
#include "TextureDescriptorObject.h"
#include "../../Vulkan/VulkanRenderer.h"

D3D::TextureDescriptorObject::TextureDescriptorObject()
	:DescriptorObject(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
{
	auto& renderer{ VulkanRenderer::GetInstance() };
	m_PlaceholderImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	m_PlaceholderImageInfo.imageView = renderer.GetDefaultImageView();
	m_PlaceholderImageInfo.sampler = renderer.GetSampler();
}

D3D::TextureDescriptorObject::TextureDescriptorObject(Texture& texture)
	:DescriptorObject(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)

{
	// Add the texture to the list of textures
	m_Textures.push_back(texture);

	// Set up the image infos
	SetupImageInfos();
}

D3D::TextureDescriptorObject::TextureDescriptorObject(std::initializer_list<const std::string>& filePaths)
	:DescriptorObject(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
{
	// Set up all textures
	SetupTextures(filePaths);

	// Set up the image infos
	SetupImageInfos();
}

D3D::TextureDescriptorObject::TextureDescriptorObject(const std::string& filePath)
	:DescriptorObject(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
{
	auto& renderer{ VulkanRenderer::GetInstance() };

	m_Textures.clear();
	m_Textures.resize(1);

	// Create texture
	renderer.CreateTexture(m_Textures[0], filePath);

	SetupImageInfos();
}

D3D::TextureDescriptorObject::~TextureDescriptorObject()
{
	// Get the device and clean up all the textures

	auto device{ VulkanRenderer::GetInstance().GetDevice() };

	for (auto& texture : m_Textures)
	{
		texture.Cleanup(device);
	}
}

void D3D::TextureDescriptorObject::AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int /*index*/)
{
	if (m_ImageInfos.size() > 0)
	{
		// Resize the descriptor writes vector
		descriptorWrites.resize(binding + m_Textures.size());

		int index{};

		// Loop trough all the image infos
		for (auto& imageInfo : m_ImageInfos)
		{
			// Create new binding
			auto& currentBinding{ descriptorWrites[binding] };

			currentBinding.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

			// Set all fields of current binding with correct values
			currentBinding.dstBinding = binding;
			currentBinding.dstArrayElement = index;
			currentBinding.descriptorType = m_Type;
			currentBinding.descriptorCount = 1;
			currentBinding.pImageInfo = &imageInfo;
			currentBinding.dstSet = descriptorSet;

			index++;
		}
	}
	else
	{
		// Resize the descriptor writes vector
		descriptorWrites.resize(binding + 1);
		// Create new binding
		auto& currentBinding{ descriptorWrites[binding] };

		currentBinding.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

		// Set all fields of current binding with correct values
		currentBinding.dstBinding = binding;
		currentBinding.dstArrayElement = 0;
		currentBinding.descriptorType = m_Type;
		currentBinding.descriptorCount = 1;
		currentBinding.pImageInfo = &m_PlaceholderImageInfo;
		currentBinding.dstSet = descriptorSet;

	}

	binding++;
}

void D3D::TextureDescriptorObject::SetupTextures(std::initializer_list<const std::string>& filePaths)
{
	// Resize textures to textureAmount
	m_Textures.resize(filePaths.size());

	// Initialize index variable
	int index{};

	auto& renderer{ VulkanRenderer::GetInstance() };

	// Loop trough all filePaths
	for (const auto& path : filePaths)
	{
		// Create texture
		renderer.CreateTexture(m_Textures[index], path);

		// Increment index
		++index;
	}
}

void D3D::TextureDescriptorObject::SetupImageInfos()
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
