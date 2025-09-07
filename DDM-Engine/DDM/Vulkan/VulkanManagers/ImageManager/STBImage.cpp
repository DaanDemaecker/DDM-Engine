// STBImage.cpp

// Header include
#include "STBImage.h"

// File includes
#include "Managers/ConfigManager.h"

// Standard library includes
#include <stdexcept>
#include <vector>

DDM::STBImage::STBImage(const std::string&& filePath)
	// Propagate to lvalue constructor
	:STBImage(filePath)
{
}

DDM::STBImage::STBImage(const std::string& filePath)
{
	// Load in the image
	stbi_uc* raw = stbi_load(filePath.c_str(), &m_TexWidth, &m_TexHeight, &m_TexChannels, m_DesiredChannels);

	// If image is not loaded in, load in default image
	if (!raw)
	{
		raw = stbi_load(ConfigManager::GetInstance().GetString("DefaultTextureName").c_str(), &m_TexWidth, &m_TexHeight, &m_TexChannels, m_DesiredChannels);
	
		// If image failed to load again, throw error
		if (!raw)
		{
			throw std::runtime_error("Failed to load texture image!");
		}
	}

	// Asign 
	m_Pixels.assign(raw, raw + m_TexWidth * m_TexHeight * m_DesiredChannels);
}

DDM::STBImage::~STBImage()
{

}

uint32_t DDM::STBImage::GetSize() const
{
	// Multiply width, height and texchannels
	return static_cast<uint32_t>(m_TexWidth * m_TexHeight * m_DesiredChannels);
}

void DDM::STBImage::Resize(uint32_t newWidth, uint32_t newHeight)
{
	std::vector<stbi_uc> resized(newWidth * newHeight * m_DesiredChannels);

	stbir_resize_uint8(
		m_Pixels.data(), m_TexWidth, m_TexHeight, 0,
		resized.data(), newWidth, newHeight, 0,
		m_DesiredChannels);

	m_Pixels.swap(resized);
	

	m_TexWidth = newWidth;
	m_TexHeight = newHeight;
}
