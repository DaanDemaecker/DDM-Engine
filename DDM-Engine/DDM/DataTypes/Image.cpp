// Image.cpp

// Header include
#include "Image.h"

// File includes
#include "Vulkan/VulkanObject.h"

int DDM::Image::s_ImageCount = 0;

DDM::Image::Image()
{
	m_ID = s_ImageCount;

	if (m_ID == 7)
	{
		std::cout << "whyyyy \n";
	}

	std::cout << "Created image: " << s_ImageCount++ << std::endl;
}

DDM::Image::~Image()
{
	std::cout << "Deleted image " << m_ID << std::endl;

	Cleanup();
}

void DDM::Image::SetImage(VkImage image)
{
	m_Image = image;
}

void DDM::Image::SetImageView(VkImageView imageView)
{
	m_ImageView = imageView;
}

void DDM::Image::SetImageMemory(VkDeviceMemory imageMemory)
{
	m_ImageMemory = imageMemory;
}

void DDM::Image::SetMipLevels(uint32_t mipLevels)
{
	m_MipLevels = mipLevels;
}

VkImage DDM::Image::GetImage() const
{
	return m_Image;
}

VkImageView DDM::Image::GetImageView() const
{
	return m_ImageView;
}

uint32_t DDM::Image::GetMipLevels() const
{
	return m_MipLevels;
}

void DDM::Image::Cleanup()
{
	if (m_ID == 1)
	{
		std::cout << "whyyyy \n";
	}


	auto device = VulkanObject::GetInstance().GetDevice();

	if (m_ImageView != VK_NULL_HANDLE)
	{
		// Destroy the image view
		vkDestroyImageView(device, m_ImageView, nullptr);
		m_ImageView = VK_NULL_HANDLE;
	}

	if (m_Image != VK_NULL_HANDLE)
	{
		// Destroy the image
		vkDestroyImage(device, m_Image, nullptr);
		m_Image = VK_NULL_HANDLE;
	}

	if (m_ImageMemory != VK_NULL_HANDLE)
	{
		// Free the memory
		vkFreeMemory(device, m_ImageMemory, nullptr);
		m_ImageMemory = VK_NULL_HANDLE;
	}
}