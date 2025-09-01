// STBImage.h
// This class will serve as a wrapper for a single image from the STB library

// File includes
#include "Includes/STBIncludes.h"

// Standard library include
#include <string>

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
		int GetWidth() const { return m_TexWidth; }

		/// <summary>
		/// Get the height of the image
		/// </summary>
		/// <returns>Height of the image</returns>
		int GetHeight() const { return m_TexHeight; }

		/// <summary>
		/// Get the amount of texture channels from the image
		/// </summary>
		/// <returns>Amount of texture channels of the image</returns>
		int GetChannels() const { return m_TexChannels; }

		/// <summary>
		/// Get a pointer to the array of pixels
		/// </summary>
		/// <returns>Pointer to the array of pixels</returns>
		stbi_uc* GetPixels() const { return m_Pixels; }


	private:
		// Width of the image
		int m_TexWidth{};

		// Height of the image
		int m_TexHeight{};

		// Amount of texture channels in the image
		int m_TexChannels{};

		// List of pixels of the image
		stbi_uc* m_Pixels{};
	};
}