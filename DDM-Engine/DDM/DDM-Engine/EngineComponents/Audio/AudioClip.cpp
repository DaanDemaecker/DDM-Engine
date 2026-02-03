// AudioClip.cpp

// Header include
#include "AudioClip.h"

DDM::AudioClip::AudioClip(const std::string& path)
	:m_FilePath{path}
{
}

DDM::AudioClip::AudioClip(const std::string&& path)
	:AudioClip(path)
{
}