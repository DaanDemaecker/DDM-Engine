// Header include
#include "FmodSoundSystem.h"

// File includes
#include "Includes/FmodIncludes.h"

// Standard library includes
#include <iostream>
#include <unordered_map>

namespace DDM
{
	class FmodImpl
	{
	public:
		FmodImpl()
		{
			FMOD_RESULT result;

			result = FMOD::System_Create(&m_pSystem);

			if (result != FMOD_OK)
			{
				HandleError(result);
			}

			m_pSystem->init(m_MaxChannels, FMOD_INIT_NORMAL, nullptr);

			m_Channels.resize(m_MaxChannels);
		}

		~FmodImpl()
		{
			for(auto& sound : m_Clips)
			{
				sound.second->release();
				sound.second = nullptr;
			}


			m_pSystem->close();
			m_pSystem->release();
		}
		
		void Update()
		{
			m_pSystem->update();
		}

		void PlayClip(const std::string& fileName)
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

			m_pSystem->playSound(m_Clips[fileName], nullptr, m_IsPaused,  &m_Channels[channelIndex]);

			m_Channels[channelIndex]->setVolume((m_MasterVolume)/m_MaxTotalVolume);
		}

		void SetMute(bool mute)
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

		void ToggleMute()
		{
			SetMute(!m_IsMuted);
		}

		void PauseAll()
		{
			SetPaused(true);
		}

		void ResumeAll()
		{
			SetPaused(false);
		}

		void SetMasterVolume(float volume)
		{
			m_MasterVolume = std::min(volume, m_MaxPartialVolume);
			SetVolume();
		}

		bool IsMuted() const
		{
			return m_IsMuted;
		}

		void LoadClip(const std::string& filePath)
		{
			CreateClip(filePath);
		}

	private:
		const int m_MaxChannels{ 32 };

		const float m_MaxTotalVolume{ 100 };

		const float m_MaxPartialVolume{ std::sqrt(m_MaxTotalVolume) };

		bool m_IsMuted{ false };

		bool m_IsPaused{ false };

		float m_MasterVolume{10};

		// FMOD core system
		FMOD::System* m_pSystem;

		// List of FMOD clips
		std::unordered_map<std::string, FMOD::Sound*> m_Clips{};

		// List of available channels
		std::vector<FMOD::Channel*> m_Channels{};

		void HandleError(FMOD_RESULT result)
		{
			std::cout << "Fmod error: " << result << "\n";
		}

		void CreateClip(const std::string& fileName)
		{
			std::cout << "Creating clip: " << fileName << "\n";
			
			m_pSystem->createSound(fileName.c_str(), FMOD_DEFAULT, nullptr, &m_Clips[fileName]);
		}

		int GetFreeChannel()
		{
			FMOD_RESULT result{};

			for(int i{}; i < m_Channels.size(); ++i)
			{
				bool isPlaying = false;

				if (m_Channels[i] == nullptr)
				{
					return i;
				}

				result = m_Channels[i]->isPlaying(&isPlaying);

				if (result != FMOD_OK)
				{
					HandleError(result);
				}

				if (!isPlaying)
				{
					return i;
				}
			}

			return -1;
		}

		void SetPaused(bool paused)
		{
			m_IsPaused = paused;

			for (auto& channel : m_Channels)
			{
				channel->setPaused(m_IsPaused);
			}
		}

		void SetVolume()
		{
			for (int i{}; i < m_Channels.size(); ++i)
			{
				m_Channels[i]->setVolume(m_MasterVolume / m_MaxTotalVolume);
			}
		}
	};
}


DDM::FmodSoundSystem::FmodSoundSystem()
{
	m_pImpl = std::make_unique<FmodImpl>();
}

void DDM::FmodSoundSystem::LoadClip(const AudioClip* clip)
{
	m_pImpl->LoadClip(clip->GetFilePath());
}

void DDM::FmodSoundSystem::PlayClip(const AudioClip* clip)
{
	m_pImpl->PlayClip(clip->GetFilePath());
}

void DDM::FmodSoundSystem::ToggleMute()
{
	m_pImpl->ToggleMute();
}

bool DDM::FmodSoundSystem::IsMuted() const
{
	return m_pImpl->IsMuted();
}

void DDM::FmodSoundSystem::SetMute(bool mute)
{
	m_pImpl->SetMute(mute);
}

void DDM::FmodSoundSystem::Update()
{
	m_pImpl->Update();
}

void DDM::FmodSoundSystem::PauseAll()
{
	m_pImpl->PauseAll();
}

void DDM::FmodSoundSystem::ResumeAll()
{
	m_pImpl->ResumeAll();
}

void DDM::FmodSoundSystem::SetMasterVolume(float volume)
{
	m_pImpl->SetMasterVolume(volume);
}
