// Attachment.h
// This class will serve as an attachment for the renderpass and framebuffers

#ifndef _ATTACHMENT_
#define _ATTACHMENT_

// File includes
#include "Includes/VulkanIncludes.h"

#include "DataTypes/Structs.h"

// Standard library includes

namespace DDM3
{
	class Attachment final
	{
	public:
		Attachment();
		~Attachment();

		Attachment(Attachment&) = delete;
		Attachment(Attachment&&) = delete;

		Attachment& operator=(Attachment&) = delete;
		Attachment& operator=(Attachment&&) = delete;

		void SetAttachmentDesc(VkAttachmentDescription desc) { m_AttachmentDesc = desc; }
		void SetAttachmentRef(VkAttachmentReference ref) { m_AttachmentRef = ref; }

		void SetAttachmentRefIndex(int index) { m_AttachmentRef.attachment = index; }

		const Texture& GetTexture() const { return m_Texture; }

		VkAttachmentDescription& GetAttachmentDesc() { return m_AttachmentDesc; }

		VkAttachmentReference& GetAttachmentRef() { return m_AttachmentRef; }

		void SetFormat(VkFormat format) { m_Format = format; }

		void SetupColorTexture(VkExtent2D extent);

		void SetupDepthImage(VkExtent2D extent);
	private:
		VkAttachmentDescription m_AttachmentDesc{};
		VkAttachmentReference m_AttachmentRef{};

		Texture m_Texture{};

		VkFormat m_Format{};

	};
}

#endif // !_ATTACHMENT_
