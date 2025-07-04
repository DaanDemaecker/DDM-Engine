// Attachment.h
// This class will serve as an attachment for the renderpass and framebuffers

#ifndef _ATTACHMENT_
#define _ATTACHMENT_

// File includes
#include "Includes/VulkanIncludes.h"

#include "DataTypes/Structs.h"

// Standard library includes
#include <memory>

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

		std::shared_ptr<Texture> GetTextureSharedPtr() const { return m_Texture; }

		const Texture* GetTexture() const { return m_Texture.get(); };

		void SetTexture(std::shared_ptr<Texture> texture) { m_Texture = texture; m_ResetOnSetup = false; }

		VkAttachmentDescription& GetAttachmentDesc() { return m_AttachmentDesc; }

		VkAttachmentReference& GetAttachmentRef() { return m_AttachmentRef; }

		void SetFormat(VkFormat format) { m_Format = format; }

		void SetupColorTexture(VkExtent2D extent);

		void SetupColorResolveTexture(VkExtent2D extent);

		void SetupDepthImage(VkExtent2D extent);

		void SetClearColorValue(VkClearColorValue value) { m_ClearColorValue = value; }
		VkClearColorValue GetClearColorValue() const { return m_ClearColorValue; }



	private:
		VkAttachmentDescription m_AttachmentDesc{};
		VkAttachmentReference m_AttachmentRef{};

		std::shared_ptr<Texture> m_Texture{};

		VkFormat m_Format{};

		VkClearColorValue m_ClearColorValue{0,0,0,1};

		bool m_ResetOnSetup{ true };

		void Cleanup();

	};
}

#endif // !_ATTACHMENT_
