// InputAttachmentDescriptorObject.h

#ifndef _INPUT_ATTACHMENT_DESCRIPTOR_OBJECT_
#define _INPUT_ATTACHMENT_DESCRIPTOR_OBJECT_

// Parent include 
#include "DescriptorObject.h"

// File includes

// Standard library includes

namespace DDM
{
	class InputAttachmentDescriptorObject final : public DescriptorObject
	{
	public:
		InputAttachmentDescriptorObject();

		void AddImageView(VkImageView imageView, VkImageLayout layout);

		void ClearImageViews();

		// Add the descriptor write objects to the list of descriptorWrites
		// Parameters:
		//     descriptorSet: the current descriptorset connected to this descriptor object
		//     descriptorWrites: the list of descriptorWrites this function will add to
		//     binding: the current binding in the shader files
		//     amount: the amount of descriptors for the current binding
		//     index: the current frame index of the renderer
		virtual void AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int amount, int index) override;

	private:
		// List of image info per texture
		std::vector<VkDescriptorImageInfo> m_ImageInfos{};



		VkDescriptorImageInfo m_PlaceholderImageInfo{ VkDescriptorImageInfo() };
	};
}

#endif // !_INPUT_ATTACHMENT_DESCRIPTOR_OBJECT_
