// Image.h
// This class will represent a single vulkan image

#ifndef _IMAGE_
#define _IMAGE_

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM
{
	class Image final
	{
	public:
		Image();

		Image(const std::string&& filepath);

		Image(const std::string& filepath);

		~Image();

		void SetImage(VkImage image);

		void SetImageView(VkImageView imageView);

		void SetImageMemory(VkDeviceMemory imageMemory);

		void SetMipLevels(uint32_t mipLevels);

		VkImage GetImage() const;

		VkImageView GetImageView() const;

		uint32_t GetMipLevels() const;

		void Cleanup();

		void SetShouldClear(bool shouldClear) { m_ShouldClear = shouldClear; }
	private:
		// Indicates wether allocated objects should be freed
		bool m_ShouldClear{ true };

		// VkImage object
		VkImage m_Image{};

		// VkDeviceMemory object
		VkDeviceMemory m_ImageMemory{};

		// VkImageView object
		VkImageView m_ImageView{};

		// The amount of levels the mipmap will have
		uint32_t m_MipLevels{1};
	};
}

#endif // !_IMAGE_
