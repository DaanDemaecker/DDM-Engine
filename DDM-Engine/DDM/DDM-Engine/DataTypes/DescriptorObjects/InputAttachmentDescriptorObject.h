// InputAttachmentDescriptorObject.h

#ifndef _DDM_INPUT_ATTACHMENT_DESCRIPTOR_OBJECT_
#define _DDM_INPUT_ATTACHMENT_DESCRIPTOR_OBJECT_

// Parent include 
#include "DescriptorObject.h"

// File includes

// Standard library includes

namespace DDM
{
	class InputAttachmentDescriptorObject final : public DescriptorObject
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		InputAttachmentDescriptorObject();

		/// <summary>
		/// Add an image view and layout to the list
		/// </summary>
		/// <param name="imageView: ">image view to add</param>
		/// <param name="layout: ">the layout the image view is in</param>
		void AddImageView(VkImageView imageView, VkImageLayout layout);

		/// <summary>
		/// Clear the list of image views
		/// </summary>
		void ClearImageViews();

		/// <summary>
		/// Add the descriptor write objets to the list of descriptorWrites
		/// </summary>
		/// <param name="descriptorSet: ">current descriptorset connected to this descriptor object</param>
		/// <param name="descriptorWrites: ">list of descriptorWrites to add to</param>
		/// <param name="binding: ">current binding in the shader files</param>
		/// <param name="amount: ">amount of elements in the array at this binding</param>
		/// <param name="index">index of the current frame in flight</param>
		virtual void AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int amount, int index) override;

	private:
		// List of image info per texture
		std::vector<VkDescriptorImageInfo> m_ImageInfos{};

		// Default image info
		VkDescriptorImageInfo m_PlaceholderImageInfo{ VkDescriptorImageInfo() };
	};
}

#endif // !_DDM_INPUT_ATTACHMENT_DESCRIPTOR_OBJECT_
