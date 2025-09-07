// STBImage.h
// This class will serve as a wrapper for a single image from the STB library

// File includes
#include "Includes/STBIncludes.h"

// Standard library include
#include <string>
#include <vector>

namespace DDM
{
	class STBImage final
	{
	public:
		// Delete default constructor
		STBImage() = delete;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="filePath: ">path to the image to load in</param>
		STBImage(const std::string&& filePath);
		
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="filePath: ">path to the image to load in</param>
		STBImage(const std::string& filePath);

		/// <summary>
		/// Destructor
		/// </summary>
		~STBImage();

		// Rule of five
		STBImage(STBImage&) = delete;
		STBImage(STBImage&&) = delete;

		STBImage& operator=(STBImage&) = delete;
		STBImage& operator=(STBImage&&) = delete;

		/// <summary>
		/// Get the width of the image
		/// </summary>
		/// <returns>Width of the image</returns>
		uint32_t GetWidth() const { return static_cast<uint32_t>(m_TexWidth); }

		/// <summary>
		/// Get the height of the image
		/// </summary>
		/// <returns>Height of the image</returns>
		uint32_t GetHeight() const { return static_cast<uint32_t>(m_TexHeight); }

		/// <summary>
		/// Get the amount of texture channels from the image
		/// </summary>
		/// <returns>Amount of texture channels of the image</returns>
		uint32_t GetChannels() const { return static_cast<uint32_t>(m_TexChannels); }

		/// <summary>
		/// Get a pointer to the array of pixels
		/// </summary>
		/// <returns>Pointer to the array of pixels</returns>
		stbi_uc* GetPixels() { return static_cast<stbi_uc*>(m_Pixels.data()); }

		/// <summary>
		/// Get the size of the image
		/// </summary>
		/// <returns>Size of the image</returns>
		uint32_t GetSize() const;

		/// <summary>
		/// Resizes the image
		/// </summary>
		/// <param name="newWidth: ">The new width for the image</param>
		/// <param name="newHeight: ">The new height for the image</param>
		void Resize(uint32_t newWidth, uint32_t newHeight);
	private:
		// Width of the image
		int m_TexWidth{};

		// Height of the image
		int m_TexHeight{};

		// Amount of texture channels in the image
		int m_TexChannels{};

		// Desired amount of channels in image
		int m_DesiredChannels{ STBI_rgb_alpha };

		// List of pixels of the image
		std::vector<stbi_uc> m_Pixels{};
	};
}