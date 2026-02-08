// FmodSystem.cpp

// Header include
#include "FmodSystem.h"

// File includes
#include "FmodErrorHandler.h"
#include "FmodChannel.h"

#include "DDM-Engine/EngineComponents/Audio/AudioEvents.h"
#include "DDM-Engine/EngineComponents/Audio/AudioSourceInfo.h"
#include "DDM-Engine/EngineComponents/Transform.h"

#include "DDM-Engine/BaseClasses/GameObject.h"

// Standard library includes
#include <algorithm>

DDM::FmodSystem::FmodSystem(int maxChannels)
	:m_MaxChannels{maxChannels}
{
	HandleError(FMOD::System_Create(&m_pSystem));

	m_pSystem->init(m_MaxChannels, FMOD_INIT_NORMAL, nullptr);

	m_Channels.resize(maxChannels);
}

DDM::FmodSystem::~FmodSystem()
{
	for (auto& channel : m_Channels)
	{
		if (channel != nullptr)
		{
			channel->Stop();
		}
	}

	m_Channels.clear();

	for (auto& sound : m_Clips)
	{
		sound.second->release();
		sound.second = nullptr;
	}
	m_Clips.clear();

	FMOD::ChannelGroup* master = nullptr;
	m_pSystem->getMasterChannelGroup(&master);
	if (master) master->stop();

	m_pSystem->mixerSuspend();

	m_pSystem->close();

	m_pSystem->release();
	m_pSystem = nullptr;
}

void DDM::FmodSystem::Update()
{
	m_pSystem->update();

	for (auto index : m_ChannelsToRemove)
	{
		m_Channels[index] = nullptr;
	}

	m_ChannelsToRemove.clear();
}

int DDM::FmodSystem::PlayClip(const std::string& fileName, const AudioSourceInfo& info, Observer* observer)
{
	if (m_Clips[fileName] == nullptr)
	{
		CreateClip(fileName);
	}

	if (m_Clips[fileName] == nullptr)
	{
		std::cout << "Failed to load sound file: " << fileName << std::endl;
		return -1;
	}

	int channelIndex = info.Channel;

	if (channelIndex < 0)
	{
		channelIndex = GetFreeChannel(info.Priority);
	}


	if (channelIndex < 0)
	{
		std::cout << "No available channel \n";
		return channelIndex;
	}

	if (m_Channels[channelIndex] != nullptr)
	{
		m_Channels[channelIndex]->Stop(true);
	}

	FMOD::Channel* newChannel;
	m_pSystem->playSound(m_Clips[fileName], nullptr, false, &newChannel);

	std::cout << "Playing in channel: " << channelIndex << "\n";

	newChannel->setLoopCount(0);

	if (m_Channels[channelIndex] == nullptr)
	{
		m_Channels[channelIndex] = std::make_unique<FmodChannel>(newChannel, channelIndex, m_Info, info);
		m_Channels[channelIndex]->AddObserver(this);
		m_Channels[channelIndex]->AddObserver(observer);
	}
	else
	{
		m_Channels[channelIndex]->SetChannel(newChannel, m_Info, info);
	}

	return channelIndex;
}

void DDM::FmodSystem::SetMasterMute(bool mute)
{
	m_Info.Muted = mute;

	for (auto& channel : m_Channels)
	{
		if (channel == nullptr)
		{
			continue;
		}

		channel->SetMasterMute(m_Info.Muted);
	}
}

void DDM::FmodSystem::ToggleMute()
{
	SetMasterMute(!m_Info.Muted);
}

void DDM::FmodSystem::SetMasterVolume(float volume)
{
	m_Info.Volume = std::min(volume, m_MaxVolume);
	
	for (auto& channel : m_Channels)
	{
		if (channel != nullptr)
		{
			channel->SetMasterVolume(m_Info.Volume);
		}
	}
}

float DDM::FmodSystem::GetMasterVolume() const 
{
	return m_Info.Volume;
}

bool DDM::FmodSystem::IsMuted() const
{
	return m_Info.Muted;
}

void DDM::FmodSystem::LoadClip(const std::string& filePath)
{
	CreateClip(filePath);
}

void DDM::FmodSystem::SetVolume(const AudioSourceInfo& info)
{
	if (!IsValidChannel(info.Channel))
	{
		return;
	}

	m_Channels[info.Channel]->SetVolume(info.Volume);
}

float DDM::FmodSystem::GetVolume(const AudioSourceInfo& info)
{
	if (!IsValidChannel(info.Channel))
	{
		return 1.0f;
	}

	return m_Channels[info.Channel]->GetVolume();
}

void DDM::FmodSystem::Notify(const Event& event)
{
	if (auto audioFinishedEvent{ dynamic_cast<const AudioFinishedEvent*>(&event) })
	{
		m_ChannelsToRemove.push_back(audioFinishedEvent->Index);
	}
}

void DDM::FmodSystem::SetMute(const AudioSourceInfo& info)
{
	if (!IsValidChannel(info.Channel))
	{
		return;
	}

	m_Channels[info.Channel]->SetMute(info.Muted);
}

void DDM::FmodSystem::UpdateSourceLocation(const AudioSourceInfo& info, GameObject* pGameObject)
{
	if (!IsValidChannel(info.Channel) || pGameObject == nullptr)
	{
		return;
	}

	m_Channels[info.Channel]->UpdateSourceLocation(pGameObject);
}

void DDM::FmodSystem::UpdateListenerLocation(GameObject* pGameObject)
{
	if (pGameObject == nullptr || m_pSystem == nullptr)
	{
		return;
	}

	auto transform = pGameObject->GetTransform();

	auto pos = transform->GetWorldPosition();

	auto forward = transform->GetForward();

	auto up = transform->GetUp();

	FMOD_VECTOR audioPos{ pos.x, pos.y, pos.z };

	FMOD_VECTOR audioVel{};

	FMOD_VECTOR audioForward{ forward.x, forward.y, forward.z };

	FMOD_VECTOR audioUp{ up.x, up.y, up.z };


	m_pSystem->set3DListenerAttributes(0, &audioPos, &audioVel, &audioForward, &audioUp);

}

void DDM::FmodSystem::SetPaused(const AudioSourceInfo& info)
{
	if (!IsValidChannel(info.Channel))
	{
		return;
	}
	m_Channels[info.Channel]->SetPaused(info.Paused);
}

void DDM::FmodSystem::Set3D(const AudioSourceInfo& info)
{
	if (!IsValidChannel(info.Channel))
	{
		return;
	}

	m_Channels[info.Channel]->Set3D(info.Is3D);
}

void DDM::FmodSystem::SetMaster3D(bool is3D)
{
	m_Info.Is3D = is3D;

	for (auto& channel : m_Channels)
	{
		if (channel != nullptr)
		{
			channel->SetMaster3D(m_Info.Is3D);
		}
	}
}

bool DDM::FmodSystem::GetMaster3D()
{
	return m_Info.Is3D;
}

void DDM::FmodSystem::Stop(AudioSourceInfo& info)
{
	if (!IsValidChannel(info.Channel))
	{
		return;
	}

	m_Channels[info.Channel]->Stop();
}

void DDM::FmodSystem::StopAll()
{
	for (auto& channel : m_Channels)
	{
		if (channel == nullptr)
		{
			continue;
		}

		channel->Stop();
	}
}

void DDM::FmodSystem::SetLoop(AudioSourceInfo& sourceInfo)
{
	if (!IsValidChannel(sourceInfo.Channel))
	{
		return;
	}

	m_Channels[sourceInfo.Channel]->SetLoop(sourceInfo.Looping);
}

void DDM::FmodSystem::SetMasterFrequency(float frequency)
{
	m_Info.Frequency = frequency;

	for (auto& channel : m_Channels)
	{
		if (channel != nullptr)
		{
			channel->SetMasterFrequency(m_Info.Frequency);
		}
	}
}

void DDM::FmodSystem::SetFrequency(AudioSourceInfo& sourceInfo)
{
	if (!IsValidChannel(sourceInfo.Channel))
	{
		return;
	}

	m_Channels[sourceInfo.Channel]->SetFrequency(sourceInfo.Frequency);
}

void DDM::FmodSystem::SetPriority(AudioSourceInfo& sourceInfo)
{
	if (!IsValidChannel(sourceInfo.Channel))
	{
		return;
	}

	m_Channels[sourceInfo.Channel]->SetPriority(sourceInfo.Priority);
}

void DDM::FmodSystem::SetMasterPitch(float pitch)
{
	m_Info.Pitch = pitch;
	for (auto& channel : m_Channels)
	{
		if (channel != nullptr)
		{
			channel->SetMasterPitch(m_Info.Pitch);
		}
	}
}

void DDM::FmodSystem::SetPitch(AudioSourceInfo& sourceInfo)
{
	if (!IsValidChannel(sourceInfo.Channel))
	{
		return;
	}

	m_Channels[sourceInfo.Channel]->SetPitch(sourceInfo.Pitch);
}

int DDM::FmodSystem::GetFreeChannel(int priority)
{
	int lowestPriority{INT_MAX};

	int lowestIndex{};

	for (int i{}; i < m_Channels.size(); ++i)
	{
		if (m_Channels[i] == nullptr)
		{
			return i;
		}
		else
		{
			int priority = m_Channels[i]->GetPriority();

			if (priority < lowestPriority)
			{
				lowestPriority = priority;
				lowestIndex = i;
			}
		}
	}

	if (lowestPriority < priority)
	{
		return lowestIndex;
	}

	return -1;
}

void DDM::FmodSystem::CreateClip(const std::string& fileName)
{
	std::cout << "Creating clip: " << fileName << "\n";

	m_pSystem->createSound(fileName.c_str(), FMOD_LOOP_NORMAL | FMOD_3D, nullptr, &m_Clips[fileName]);
}

bool DDM::FmodSystem::IsValidChannel(int index)
{
	return index >= 0 && m_Channels[index] != nullptr;
}
