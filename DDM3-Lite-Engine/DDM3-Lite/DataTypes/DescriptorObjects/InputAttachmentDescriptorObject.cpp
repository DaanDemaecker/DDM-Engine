// InputAttachmentDescriptorObject.cpp

// Header include
#include "InputAttachmentDescriptorObject.h"

// File includes
#include "Vulkan/VulkanObject.h"

DDM3::InputAttachmentDescriptorObject::InputAttachmentDescriptorObject()
	:DescriptorObject(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
{
	auto& vulkanObject{ VulkanObject::GetInstance() };
	m_PlaceholderImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	m_PlaceholderImageInfo.imageView = vulkanObject.GetDefaultImageView();
	m_PlaceholderImageInfo.sampler = vulkanObject.GetSampler();
}

void DDM3::InputAttachmentDescriptorObject::AddImageView(VkImageView imageView, VkImageLayout layout)
{
	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageView = imageView;
	imageInfo.imageLayout = layout;
	imageInfo.sampler = VK_NULL_HANDLE;

	m_ImageInfos.push_back(imageInfo);
}

void DDM3::InputAttachmentDescriptorObject::ClearImageViews()
{
	m_ImageInfos.clear();
}

void DDM3::InputAttachmentDescriptorObject::AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int amount, int index)
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
