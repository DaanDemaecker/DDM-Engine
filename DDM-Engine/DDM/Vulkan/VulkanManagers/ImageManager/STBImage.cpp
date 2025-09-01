// STBImage.cpp

// Header include
#include "STBImage.h"

// File includes
#include "Managers/ConfigManager.h"

// Standard library includes
#include <stdexcept>

DDM::STBImage::STBImage(std::string&& filePath)
	// Propagate to lvalue constructor
	:STBImage(filePath)
{
}

DDM::STBImage::STBImage(std::string& filePath)
{
	// Load in the image
	m_Pixels = stbi_load(filePath.c_str(), &m_TexWidth, &m_TexHeight, &m_TexChannels, STBI_rgb_alpha);

	if (!m_Pixels)
	{
		m_Pixels = stbi_load(ConfigManager::GetInstance().GetString("DefaultTextureName").c_str(), &m_TexWidth, &m_TexHeight, &m_TexChannels, STBI_rgb_alpha);
	
		if (!m_Pixels)
		{
			throw std::runtime_error("Failed to load texture image!");
		}
	}
}

DDM::STBImage::~STBImage()
{
	// Only release pixels if the image was loaded in
	if (m_Pixels)
	{
		// Free the pixels
		stbi_image_free(m_Pixels);
	}
}
