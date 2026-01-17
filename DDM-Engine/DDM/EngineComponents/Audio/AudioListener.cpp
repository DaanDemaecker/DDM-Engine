// AudioListener.cpp

// Header include
#include "AudioListener.h"
#include "ServiceLocator/ServiceLocator.h"

DDM::AudioListener::AudioListener()
{
}

void DDM::AudioListener::LateUpdate()
{
	ServiceLocator::GetSoundSystem().UpdateListenerLocation(GetOwner());
}
