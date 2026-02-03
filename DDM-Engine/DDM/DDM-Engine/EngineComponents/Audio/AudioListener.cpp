// AudioListener.cpp

// Header include
#include "AudioListener.h"

// File includes
#include "DDM-Engine/ServiceLocator/ServiceLocator.h"

DDM::AudioListener::AudioListener()
{
}

void DDM::AudioListener::LateUpdate()
{
	ServiceLocator::GetSoundSystem().UpdateListenerLocation(GetOwner());
}
