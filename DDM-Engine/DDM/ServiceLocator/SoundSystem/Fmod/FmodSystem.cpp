// FmodSystem.cpp

// Header include
#include "FmodSystem.h"

// File includes
#include "FmodErrorHandler.h"

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
}

int DDM::FmodSystem::PlayClip(const std::string& fileName)
{
	if (m_Clips[fileName] == nullptr)
	{
		CreateClip(fileName);
	}

	int channelIndex = GetFreeChannel();

	if (channelIndex < 0)
	{
		std::cout << "No available channel \n";
	}

	m_pSystem->playSound(m_Clips[fileName], nullptr, false, &m_Channels[channelIndex]);

	std::cout << "Playing in channel: " << channelIndex << "\n";

	m_Channels[channelIndex]->setVolume(m_MasterVolume);
	m_Channels[channelIndex]->setLoopCount(0);
	m_Channels[channelIndex]->setMute(m_IsMuted);

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

		channel->setMute(m_IsMuted);
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

bool DDM::FmodSystem::IsMuted() const
{
	return m_IsMuted;
}

void DDM::FmodSystem::LoadClip(const std::string& filePath)
{
	CreateClip(filePath);
}

int DDM::FmodSystem::GetFreeChannel()
{
	for (int i{}; i < m_Channels.size(); ++i)
	{
		bool isPlaying = false;

		if (m_Channels[i] == nullptr)
		{
			return i;
		}

		HandleError(m_Channels[i]->isPlaying(&isPlaying));

		if (!isPlaying)
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
	for(auto channel : m_Channels)
	{
		channel->setVolume(m_MasterVolume);
	}
}