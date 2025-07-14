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
		Attachment(int swapchainImagesAmount);

		Attachment() = delete;

		~Attachment();

		Attachment(Attachment&) = delete;
		Attachment(Attachment&&) = delete;

		Attachment& operator=(Attachment&) = delete;
		Attachment& operator=(Attachment&&) = delete;

		void SetAttachmentDesc(VkAttachmentDescription desc) { m_AttachmentDesc = desc; }

		std::shared_ptr<Texture> GetTextureSharedPtr(int swapchainIndex) const { return m_Textures[swapchainIndex]; }

		const Texture* GetTexture(int swapchainIndex) const { return m_Textures[swapchainIndex].get(); };

		void SetTexture(std::shared_ptr<Texture> texture, int index) { m_Textures[index] = texture; m_ResetOnSetup = false; }

		void SetTexture(int index, VkImage image, VkImageView imageView);

		VkAttachmentDescription& GetAttachmentDesc() { return m_AttachmentDesc; }

		void SetIsInput(bool isInput) { m_IsInput = isInput; }

		void SetFormat(VkFormat format) { m_Format = format; }

		void SetupImage(int index, VkExtent2D extent, VkImageView swapchainImage);

		void SetAttachmentType(int type) { m_Type = type; }

		int GetAttachmentType() const { return m_Type; }

		void SetClearColorValue(VkClearColorValue value) { m_ClearColorValue = value; }
		VkClearColorValue GetClearColorValue() const { return m_ClearColorValue; }

		void SetClearDepthStencilValue(VkClearDepthStencilValue value) { m_ClearDepthstencilValue = value; }
		VkClearDepthStencilValue GetClearDepthStencilValue() const { return m_ClearDepthstencilValue; }

		void SetIsSwapchainImage(bool isSwapchainImage) { m_IsSwapchainImage = isSwapchainImage; }
		bool IsSwapchainImage() const { return m_IsSwapchainImage; }

		enum
		{
			kAttachmentType_Color = 0,
			kAttachmentType_ColorResolve = 1,
			kAttachmentType_DepthStencil = 2,
		};
	private:
		bool m_IsSwapchainImage{ false };

		bool m_IsInput{ false };

		int m_Type = kAttachmentType_Color;

		VkAttachmentDescription m_AttachmentDesc{};

		std::vector<std::shared_ptr<Texture>> m_Textures{};

		VkFormat m_Format{};

		VkClearColorValue m_ClearColorValue{0,0,0,1};
		VkClearDepthStencilValue m_ClearDepthstencilValue{ 1, 0};

		bool m_ResetOnSetup{ true };

		void Cleanup();


		void SetupColorTexture(int index, VkExtent2D extent);

		void SetupDepthImage(int index, VkExtent2D extent);

		void CleanupImage(int index);
	};
}

#endif // !_ATTACHMENT_
