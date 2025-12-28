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
			for(auto& sound : m_Sounds)
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

		void Play(std::string& fileName)
		{
			if (m_Sounds[fileName] == nullptr)
			{
				CreateSound(fileName);
			}

			int channelIndex = GetFreeChannel();

			m_pSystem->playSound(m_Sounds[fileName], nullptr, false,  &m_Channels[channelIndex]);
		}

	private:
		const int m_MaxChannels{ 32 };

		// FMOD core system
		FMOD::System* m_pSystem;

		// List of FMOD sounds
		std::unordered_map<std::string, FMOD::Sound*> m_Sounds{};

		// List of available channels
		std::vector<FMOD::Channel*> m_Channels{};

		void HandleError(FMOD_RESULT result)
		{
			std::cout << "Fmod error: " << result << "\n";
		}

		void CreateSound(std::string& fileName)
		{
			std::cout << fileName << "\n";

			m_pSystem->createSound(fileName.c_str(), FMOD_DEFAULT, nullptr, &m_Sounds[fileName]);
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

			return 0;
		}
	};
}


DDM::FmodSoundSystem::FmodSoundSystem()
{
	m_pImpl = std::make_unique<FmodImpl>();
}

void DDM::FmodSoundSystem::Play(std::string& fileName)
{
	m_pImpl->Play(fileName);
}

void DDM::FmodSoundSystem::Update()
{
	m_pImpl->Update();
}
