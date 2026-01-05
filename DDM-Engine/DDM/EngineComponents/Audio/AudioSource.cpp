// AudioSource.cpp

// Header include
#include "AudioSource.h"

DDM::AudioSource::AudioSource()
	:Component()
{

}

void DDM::AudioSource::SetClip(const AudioClip& clip)
{
	_clip = clip;
}

void DDM::AudioSource::SetClip(const AudioClip&& clip)
{
	// Forward to l-value overloaded function
	SetClip(clip);
}

void DDM::AudioSource::SetClip(const std::string& path)
{
	// Create audioclip and forward to overloaded function
	SetClip(AudioClip(path));
}

void DDM::AudioSource::SetClip(const std::string&& path)
{
	// Forward to l-value overloaded function
	SetClip(path);
}

void DDM::AudioSource::Play()
{

}
