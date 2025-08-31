// InputAttachmentDescriptorObject.cpp

// Header include
#include "InputAttachmentDescriptorObject.h"

// File includes
#include "Vulkan/VulkanObject.h"

DDM::InputAttachmentDescriptorObject::InputAttachmentDescriptorObject()
	// Type of this object is input attachment
	:DescriptorObject(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
{
	// Set up placeholder image info
	auto& vulkanObject{ VulkanObject::GetInstance() };
	m_PlaceholderImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	m_PlaceholderImageInfo.imageView = vulkanObject.GetDefaultImageView();
	m_PlaceholderImageInfo.sampler = vulkanObject.GetSampler();
}

void DDM::InputAttachmentDescriptorObject::AddImageView(VkImageView imageView, VkImageLayout layout)
{
	// Set up new image info
	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageView = imageView;
	imageInfo.imageLayout = layout;
	imageInfo.sampler = m_PlaceholderImageInfo.sampler;

	// Add image info to the list
	m_ImageInfos.push_back(imageInfo);
}

void DDM::InputAttachmentDescriptorObject::ClearImageViews()
{
	// Clear list of image infos
	m_ImageInfos.clear();
}

void DDM::InputAttachmentDescriptorObject::AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int amount, int index)
{
	// Loop through the amount of descriptors at this binding
	// This amount represents the size of the array if one is used
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

		// Set descriptorwrite destination to the correct descriptor set
		descriptorWrite.dstSet = descriptorSet;

		// Add to the list
		descriptorWrites.push_back(descriptorWrite);
	}

	// Increase binding
	++binding;
}
