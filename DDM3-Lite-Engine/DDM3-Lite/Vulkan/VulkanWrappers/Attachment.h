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

		std::shared_ptr<Texture> GetTextureSharedPtr() const { return m_Texture; }

		const Texture* GetTexture() const { return m_Texture.get(); };

		void SetTexture(std::shared_ptr<Texture> texture) { m_Texture = texture; m_ResetOnSetup = false; }

		VkAttachmentDescription& GetAttachmentDesc() { return m_AttachmentDesc; }

		void SetFormat(VkFormat format) { m_Format = format; }

		void SetupImage(VkExtent2D extent, VkImageView swapchainImage);

		void SetAttachmentType(int type) { m_Type = type; }

		int GetAttachmentType() const { return m_Type; }

		void SetClearColorValue(VkClearColorValue value) { m_ClearColorValue = value; }
		VkClearColorValue GetClearColorValue() const { return m_ClearColorValue; }

		void SetClearDepthStencilValue(VkClearDepthStencilValue value) { m_ClearDepthstencilValue = value; }
		VkClearDepthStencilValue GetClearDepthStencilValue() const { return m_ClearDepthstencilValue; }

		enum
		{
			kAttachmentType_Color = 0,
			kAttachmentType_ColorResolve = 1,
			kAttachmentType_DepthStencil = 2,
		};
	private:
		int m_Type = kAttachmentType_Color;

		VkAttachmentDescription m_AttachmentDesc{};

		std::shared_ptr<Texture> m_Texture{};

		VkFormat m_Format{};

		VkClearColorValue m_ClearColorValue{0,0,0,1};
		VkClearDepthStencilValue m_ClearDepthstencilValue{ 1, 0};

		bool m_ResetOnSetup{ true };

		void Cleanup();


		void SetupColorTexture(VkExtent2D extent);

		void SetupColorResolveTexture(VkExtent2D extent, VkImageView swapchainImage);

		void SetupDepthImage(VkExtent2D extent);
	};
}

#endif // !_ATTACHMENT_
