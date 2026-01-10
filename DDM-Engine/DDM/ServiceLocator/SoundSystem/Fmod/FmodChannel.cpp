// FmodChannel.cpp

// Header include
#include "FmodChannel.h"

// File includes

DDM::FmodChannel::FmodChannel(FMOD::Channel* pChannel)
{
	m_pChannel = pChannel;

	m_pChannel->setUserData(this);
	m_pChannel->setCallback(ChannelCallback);
}

FMOD_RESULT F_CALLBACK DDM::FmodChannel::ChannelCallback(FMOD_CHANNELCONTROL* channelControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void* commandData1, void* commandData2)
{
	if (callbackType == FMOD_CHANNELCONTROL_CALLBACK_END)
	{

	}

	return FMOD_OK;
}
