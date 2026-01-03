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

		void PlayClip(std::string& fileName)
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

			m_Channels[channelIndex]->setVolume((m_MasterVolume * m_SfxVolume)/m_MaxTotalVolume);
		}

		void PlayStream(std::string& fileName)
		{
			if (m_Streams[fileName] == nullptr)
			{
				CreateStream(fileName);
			}

			if (m_Channels[m_MusicChannel] != nullptr)
			{
				bool isPlaying{};
				FMOD_RESULT result = m_Channels[m_MusicChannel]->isPlaying(&isPlaying);

				if (result != FMOD_OK)
				{
					HandleError(result);
				}

				if (isPlaying)
				{
					result = m_Channels[m_MusicChannel]->stop();

					if (result != FMOD_OK)
					{
						HandleError(result);
					}
				}
			}

			m_pSystem->playSound(m_Streams[fileName], nullptr, m_IsPaused, &m_Channels[m_MusicChannel]);

			m_Channels[m_MusicChannel]->setVolume((m_MasterVolume * m_SfxVolume) / m_MaxTotalVolume);
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

		void SetMusicVolume(float volume)
		{
			m_MusicVolume = std::min(volume, m_MaxPartialVolume);
			SetVolume();
		}


		void SetSfxVolume(float volume)
		{
			m_SfxVolume = std::min(volume, m_MaxPartialVolume);
			SetVolume();
		}

		bool IsMuted() const
		{
			return m_IsMuted;
		}

	private:
		const int m_MaxChannels{ 32 };

		const int m_MusicChannel{ 0 };

		const float m_MaxTotalVolume{ 100 };

		const float m_MaxPartialVolume{ std::sqrt(m_MaxTotalVolume) };

		bool m_IsMuted{ false };

		bool m_IsPaused{ false };

		float m_MasterVolume{10};

		float m_MusicVolume{10};

		float m_SfxVolume{10};

		// FMOD core system
		FMOD::System* m_pSystem;

		// List of FMOD clips
		std::unordered_map<std::string, FMOD::Sound*> m_Clips{};

		// List of FMOD streams
		std::unordered_map<std::string, FMOD::Sound*> m_Streams{};

		// List of available channels
		std::vector<FMOD::Channel*> m_Channels{};

		void HandleError(FMOD_RESULT result)
		{
			std::cout << "Fmod error: " << result << "\n";
		}

		void CreateClip(std::string& fileName)
		{
			std::cout << "Creating clip: " << fileName << "\n";
			
			m_pSystem->createSound(fileName.c_str(), FMOD_LOOP_OFF, nullptr, &m_Clips[fileName]);
		}

		void CreateStream(std::string& fileName)
		{
			std::cout << "Creating stream: " << fileName << "\n";
			m_pSystem->createSound(fileName.c_str(), FMOD_LOOP_NORMAL, nullptr, &m_Streams[fileName]);
		}

		int GetFreeChannel()
		{
			FMOD_RESULT result{};

			for(int i{}; i < m_Channels.size(); ++i)
			{
				if (i == m_MusicChannel)
				{
					continue;
				}

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
				if (i == m_MusicChannel)
				{
					m_Channels[i]->setVolume((m_MasterVolume * m_MusicVolume) / m_MaxTotalVolume);
				}
				else
				{
					m_Channels[i]->setVolume((m_MasterVolume * m_SfxVolume) / m_MaxTotalVolume);
				}
			}
		}
	};
}


DDM::FmodSoundSystem::FmodSoundSystem()
{
	m_pImpl = std::make_unique<FmodImpl>();
}

void DDM::FmodSoundSystem::PlayClip(std::string& fileName)
{
	m_pImpl->PlayClip(fileName);
}

void DDM::FmodSoundSystem::PlayStream(std::string& fileName)
{
	m_pImpl->PlayStream(fileName);
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

void DDM::FmodSoundSystem::SetMusicVolume(float volume)
{
	m_pImpl->SetMusicVolume(volume);
}

void DDM::FmodSoundSystem::SetSfxVolume(float volume)
{
	m_pImpl->SetSfxVolume(volume);
}
