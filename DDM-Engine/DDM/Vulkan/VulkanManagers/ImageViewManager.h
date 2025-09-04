// ImageViewManager.h
// This class will hold and manage the color- and depthImaveViews

//File includes
#include "Includes/VulkanIncludes.h"

namespace DDM
{
	// Class forward declarations
	class ImageManager;
	class GPUObject;
	class Image;

	class ImageViewManager final
	{
	public:
		// Constructor
		// Parameters:
		//     msaaSamples: max amount of samples per pixel
		ImageViewManager(VkSampleCountFlagBits msaaSamples);

		// Delete default constructor
		ImageViewManager() = delete;

		// Dafault destructor
		~ImageViewManager();

		// Delete copy and move functions
		ImageViewManager(ImageViewManager& other) = delete;
		ImageViewManager(ImageViewManager&& other) = delete;
		ImageViewManager& operator=(ImageViewManager& other) = delete;
		ImageViewManager& operator=(ImageViewManager&& other) = delete;

		// Initialize the color image for the swapchain
		// Parameters:
		//     pGPUObject: pointer to the GPU Object
		//     format: the requested format for the color image
		//     swapchainExtent: the extent of the swapchain, for use of the image size
		//     pImageManager: pointer to the image manager
		void CreateColorResources(GPUObject* pGPUObject, VkFormat format,
			VkExtent2D swapchainExtent, DDM::ImageManager* pImageManager);

		// Initialize the depth image for the swapchain
		// Parameters:
		//     pGPUObject: pointer to the GPU Object
		//     swapchainExtent: the extent of the swapchain, for use of the image size
		//     pImageManager: pointer to the image manager
		void CreateDepthResources(GPUObject* pGPUObject, VkExtent2D swapchainExtent, DDM::ImageManager* pImageManager, VkCommandBuffer commandBuffer);

		// Get the imageView for the color image
		VkImageView GetColorImageView() const;
		// Get the imageView for the depth image
		VkImageView GetDepthImageView() const;
		// Get the max amount of samples per pixel
		VkSampleCountFlagBits GetMsaaSamples() const { return m_MsaaSamples; }

		const std::vector<std::shared_ptr<Image>>& GetColorImages() const;

	private:
		// Max amount of samples per pixel, initialize as 1
		VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

		std::vector<std::shared_ptr<Image>> m_pColorImages{};

		// Texture struct for the color image
		std::shared_ptr<Image> m_pColorImage{};

		// Texture struct for the depth image
		std::shared_ptr<Image> m_pDepthImage{};
	};
}