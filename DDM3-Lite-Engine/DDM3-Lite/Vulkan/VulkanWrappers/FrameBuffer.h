// FrameBuffer.h
// This clas serves as a wrapper for a VkFramebuffer in the swapchain

#ifndef _FRAME_BUFFER_
#define _FRAME_BUFFER_

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <vector>
#include <memory>

namespace DDM3
{
	class RenderpassWrapper;

	class FrameBuffer final
	{
	public:
		FrameBuffer();
		~FrameBuffer();

		FrameBuffer(FrameBuffer&) = delete;
		FrameBuffer(FrameBuffer&&) = delete;

		FrameBuffer& operator=(FrameBuffer&) = delete;
		FrameBuffer& operator=(FrameBuffer&&) = delete;

		VkFramebuffer GetFrameBuffer() const { return m_FrameBuffer; }

		void CreateFrameBuffer(int index, RenderpassWrapper* renderpass, VkExtent2D extent, VkImageView swapchainImage);

	public:
		VkFramebuffer m_FrameBuffer{};


	};
}

#endif // !_FRAME_BUFFER_
