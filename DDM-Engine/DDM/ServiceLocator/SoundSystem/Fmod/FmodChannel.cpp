// FmodChannel.cpp

// Header include
#include "FmodChannel.h"

// File includes
#include "EngineComponents/Audio/AudioEvents.h"
#include "FmodErrorHandler.h"
#include "BaseClasses/GameObject.h"
#include "EngineComponents/Transform.h"
#include "EngineComponents/Audio/AudioSourceInfo.h"
#include "FmodSystemInfo.h"


DDM::FmodChannel::FmodChannel(FMOD::Channel* pChannel, int index, const FmodSystemInfo& systemInfo, const AudioSourceInfo& sourceInfo)
	:m_Index{ index }
{
	m_pChannel = pChannel;

	HandleError(m_pChannel->setUserData(this));
	HandleError(m_pChannel->setCallback(ChannelCallback));

	SetInfo(systemInfo, sourceInfo);
}

DDM::FmodChannel::~FmodChannel()
{
}

void DDM::FmodChannel::SetChannel(FMOD::Channel* pChannel, const FmodSystemInfo& systemInfo, const AudioSourceInfo& sourceInfo)
{
	m_IgnoreNextCallback = true;

	Stop();

	m_pChannel = pChannel;

	SetInfo(systemInfo, sourceInfo);
}

void DDM::FmodChannel::SetMasterMute(bool muted)
{
	m_MasterMuted = muted;
	SetMute();
}

void DDM::FmodChannel::SetMute(bool muted)
{
	m_Muted = muted;
	SetMute();
}

void DDM::FmodChannel::SetMasterVolume(float volume)
{
	m_MasterVolume = volume;
	SetVolume();
}

void DDM::FmodChannel::SetVolume(float volume)
{
	m_Volume = volume;
	SetVolume();
}

float DDM::FmodChannel::GetVolume()
{
	return m_Volume;
}

void DDM::FmodChannel::UpdateSourceLocation(GameObject* pGameObject)
{
	auto transform = pGameObject->GetTransform();

	auto pos = transform->GetWorldPosition();

	FMOD_VECTOR audioPos{pos.x, pos.y, pos.z};

	FMOD_VECTOR audioVel{};

	m_pChannel->set3DAttributes(&audioPos, &audioVel);
}

void DDM::FmodChannel::SetMasterPaused(bool paused)
{
	m_MasterPaused = paused;

	SetPaused();
}

void DDM::FmodChannel::SetPaused(bool paused)
{
	m_Paused = paused;

	SetPaused();
}

void DDM::FmodChannel::SetMaster3D(bool is3d)
{
	m_Master3d = is3d;
	
	Set3D();
}

void DDM::FmodChannel::Set3D(bool is3d)
{
	m_Is3d = is3d;

	Set3D();
}

void DDM::FmodChannel::SetLoop(bool looping)
{
	m_pChannel->setLoopCount(looping ? -1 : 0);
}

void DDM::FmodChannel::Stop()
{
	m_pChannel->stop();
}

FMOD_RESULT F_CALLBACK DDM::FmodChannel::ChannelCallback(FMOD_CHANNELCONTROL* channelControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void* commandData1, void* commandData2)
{
	if (callbackType == FMOD_CHANNELCONTROL_CALLBACK_END)
	{
		void* userData = nullptr;
		FMOD::ChannelControl* pChannel = reinterpret_cast<FMOD::ChannelControl*>(channelControl);

		pChannel->getUserData(&userData);

		FmodChannel* wrapper = static_cast<FmodChannel*>(userData);
		if (wrapper && !wrapper->m_IgnoreNextCallback)
		{
			wrapper->m_IgnoreNextCallback = false;
			// Notify wrapper that playback ended
			wrapper->NotifyObservers(AudioFinishedEvent(wrapper->GetIndex()));
		}
	}

	return FMOD_OK;
}

void DDM::FmodChannel::SetVolume()
{
	m_pChannel->setVolume(m_MasterVolume * m_Volume);
}

void DDM::FmodChannel::SetMute()
{
	m_pChannel->setMute(m_MasterMuted || m_Muted);
}

void DDM::FmodChannel::SetPaused()
{
	m_pChannel->setPaused(m_Paused ||  m_MasterPaused);
}

void DDM::FmodChannel::Set3D()
{
	m_pChannel->setMode(m_Is3d && m_Master3d ? FMOD_3D : FMOD_2D);
}

void DDM::FmodChannel::SetInfo(const FmodSystemInfo& systemInfo, const AudioSourceInfo& sourceInfo)
{
	m_MasterMuted = systemInfo.Muted;
	m_MasterVolume = systemInfo.Volume;
	m_MasterPaused = systemInfo.Paused;
	m_Master3d = systemInfo.Is3D;

	m_Muted = sourceInfo.Muted;
	m_Volume = sourceInfo.Volume;
	m_Paused = sourceInfo.Paused;
	m_Is3d = sourceInfo.Is3D;

	SetMute();
	SetVolume();
	SetPaused();
	Set3D();

	SetLoop(sourceInfo.Looping);
}
