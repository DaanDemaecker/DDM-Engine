// AudioSource.cpp

// Header include
#include "AudioSource.h"

// File includes
#include "ServiceLocator/ServiceLocator.h"

DDM::AudioSource::AudioSource()
	:Component()
{

}

void DDM::AudioSource::SetClip(const std::shared_ptr<AudioClip> clip)
{
	m_pClip = clip;
}

void DDM::AudioSource::SetClip(const std::string& path)
{
	// Create audioclip and forward to overloaded function
	SetClip(std::make_shared<AudioClip>(path));
}

void DDM::AudioSource::SetClip(const std::string&& path)
{
	// Forward to l-value overloaded function
	SetClip(path);
}

void DDM::AudioSource::Play()
{
	ServiceLocator::GetSoundSystem().PlayClip(m_pClip.get());
}
