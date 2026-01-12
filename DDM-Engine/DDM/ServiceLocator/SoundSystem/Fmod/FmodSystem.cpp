// FmodSystem.cpp

// Header include
#include "FmodSystem.h"

// File includes
#include "FmodErrorHandler.h"
#include "FmodChannel.h"
#include "EngineComponents/Audio/AudioEvents.h"

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
	for (auto& sound : m_Clips)
	{
		sound.second->release();
		sound.second = nullptr;
	}

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

int DDM::FmodSystem::PlayClip(const std::string& fileName, Observer* observer, int channel)
{
	if (m_Clips[fileName] == nullptr)
	{
		CreateClip(fileName);
	}

	int channelIndex = channel;

	if (channelIndex < 0)
	{
		channelIndex = GetFreeChannel();
	}


	if (channelIndex < 0)
	{
		std::cout << "No available channel \n";
		return channelIndex;
	}

	FMOD::Channel* newChannel;
	m_pSystem->playSound(m_Clips[fileName], nullptr, false, &newChannel);

	std::cout << "Playing in channel: " << channelIndex << "\n";

	newChannel->setVolume(m_MasterVolume);
	newChannel->setLoopCount(0);
	newChannel->setMute(m_IsMuted);

	if (m_Channels[channelIndex] == nullptr)
	{
		m_Channels[channelIndex] = std::make_unique<FmodChannel>(newChannel, channelIndex);
		m_Channels[channelIndex]->AddObserver(this);
		m_Channels[channelIndex]->AddObserver(observer);
	}
	else

	{
		m_Channels[channelIndex]->SetChannel(newChannel);
	}

	return channelIndex;
}

void DDM::FmodSystem::SetMute(bool mute)
{
	m_IsMuted = mute;

	for (auto& channel : m_Channels)
	{
		if (channel == nullptr)
		{
			continue;
		}

		channel->SetMute(m_IsMuted);
	}
}

void DDM::FmodSystem::ToggleMute()
{
	SetMute(!m_IsMuted);
}

void DDM::FmodSystem::SetMasterVolume(float volume)
{
	m_MasterVolume = std::min(volume, m_MaxVolume);
	SetVolume();
}

float DDM::FmodSystem::GetMasterVolume() const 
{
	return m_MasterVolume;
}

bool DDM::FmodSystem::IsMuted() const
{
	return m_IsMuted;
}

void DDM::FmodSystem::LoadClip(const std::string& filePath)
{
	CreateClip(filePath);
}

void DDM::FmodSystem::Notify(const Event& event)
{
	if (auto audioFinishedEvent{ dynamic_cast<const AudioFinishedEvent*>(&event) })
	{
		m_ChannelsToRemove.push_back(audioFinishedEvent->Index);
	}
}

int DDM::FmodSystem::GetFreeChannel()
{
	for (int i{}; i < m_Channels.size(); ++i)
	{
		if (m_Channels[i] == nullptr)
		{
			return i;
		}
	}

	return -1;
}

void DDM::FmodSystem::CreateClip(const std::string& fileName)
{
	std::cout << "Creating clip: " << fileName << "\n";

	m_pSystem->createSound(fileName.c_str(), FMOD_LOOP_NORMAL, nullptr, &m_Clips[fileName]);
}

void DDM::FmodSystem::SetVolume()
{
	for(auto& channel : m_Channels)
	{
		if (channel == nullptr)
		{
			continue;
		}

		channel->SetVolume(m_MasterVolume);
	}
}
