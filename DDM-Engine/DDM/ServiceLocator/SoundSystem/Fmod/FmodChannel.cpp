// FmodChannel.cpp

// Header include
#include "FmodChannel.h"

// File includes
#include "EngineComponents/Audio/AudioEvents.h"
#include "FmodErrorHandler.h"

DDM::FmodChannel::FmodChannel(FMOD::Channel* pChannel, int index)
	:m_Index{index}
{
	m_pChannel = pChannel;

	HandleError(m_pChannel->setUserData(this));
	HandleError(m_pChannel->setCallback(ChannelCallback));
}

DDM::FmodChannel::~FmodChannel()
{
}

void DDM::FmodChannel::SetChannel(FMOD::Channel* pChannel)
{
	m_pChannel->stop();

	m_pChannel = pChannel;

	m_IgnoreNextCallback = true;
}

void DDM::FmodChannel::SetMute(bool muted)
{
	m_pChannel->setMute(muted);
}

void DDM::FmodChannel::SetVolume(float volume)
{
	m_pChannel->setVolume(volume);
}

FMOD_RESULT F_CALLBACK DDM::FmodChannel::ChannelCallback(FMOD_CHANNELCONTROL* channelControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void* commandData1, void* commandData2)
{
	if (callbackType == FMOD_CHANNELCONTROL_CALLBACK_END)
	{
		void* userData = nullptr;
		FMOD::ChannelControl* pChannel = reinterpret_cast<FMOD::ChannelControl*>(channelControl);

		pChannel->getUserData(&userData);

		FmodChannel* wrapper = static_cast<FmodChannel*>(userData);
		if (wrapper && wrapper->m_IgnoreNextCallback)
		{
			wrapper->m_IgnoreNextCallback = false;
			// Notify wrapper that playback ended
			wrapper->NotifyObservers(AudioFinishedEvent(wrapper->GetIndex()));
		}
	}

	return FMOD_OK;
}
