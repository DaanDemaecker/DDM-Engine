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

			m_pSystem->playSound(m_Clips[fileName], nullptr, m_Paused,  &m_Channels[channelIndex]);
		}

		void PlayStream(std::string& fileName)
		{
			if (m_Streams[fileName] == nullptr)
			{
				CreateStream(fileName);
			}

			if (m_Channels[m_StreamChannel] != nullptr)
			{
				bool isPlaying{};
				FMOD_RESULT result = m_Channels[m_StreamChannel]->isPlaying(&isPlaying);

				if (result != FMOD_OK)
				{
					HandleError(result);
				}

				if (isPlaying)
				{
					result = m_Channels[m_StreamChannel]->stop();

					if (result != FMOD_OK)
					{
						HandleError(result);
					}
				}
			}

			m_pSystem->playSound(m_Streams[fileName], nullptr, m_Paused, &m_Channels[m_StreamChannel]);
		}

		void SetMute(bool mute)
		{
			m_Muted = mute;

			for (auto& channel : m_Channels)
			{
				if (channel == nullptr)
				{
					continue;
				}
				
				channel->setMute(m_Muted);
			}
		}

		void ToggleMute()
		{
			SetMute(!m_Muted);
		}

		void PauseAll()
		{
			SetPaused(true);
		}

		void ResumeAll()
		{
			SetPaused(false);
		}

	private:
		const int m_MaxChannels{ 32 };

		const int m_StreamChannel{ 0 };

		bool m_Muted{ false };

		bool m_Paused{ false };

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
				if (i == m_StreamChannel)
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
			m_Paused = paused;

			for (auto& channel : m_Channels)
			{
				channel->setPaused(m_Paused);
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
