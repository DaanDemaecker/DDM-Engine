// AudioClip.cpp

// Header include
#include "AudioClip.h"

DDM::AudioClip::AudioClip()
{
}

DDM::AudioClip::AudioClip(const std::string& path)
	:m_FilePath{path}
{
}

DDM::AudioClip::AudioClip(const std::string&& path)
	:AudioClip(path)
{
}

DDM::AudioClip::AudioClip(const AudioClip& other)
	:AudioClip(other.m_FilePath)
{
}

DDM::AudioClip::AudioClip(const AudioClip&& other) noexcept
	:AudioClip(other.m_FilePath)
{
}

DDM::AudioClip& DDM::AudioClip::operator=(const AudioClip& other)
{
	m_FilePath = other.m_FilePath;
	return *this;
}

DDM::AudioClip& DDM::AudioClip::operator=(const AudioClip&& other) noexcept
{
	m_FilePath = other.m_FilePath;
	return *this;
}
