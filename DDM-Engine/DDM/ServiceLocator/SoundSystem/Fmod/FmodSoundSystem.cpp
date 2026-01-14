// Header include
#include "FmodSoundSystem.h"

// File includes
#include "Includes/FmodIncludes.h"
#include "FmodErrorHandler.h"
#include "FmodSystem.h"

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
			m_pSystem = std::make_unique<FmodSystem>(m_MaxChannels);
		}

		~FmodImpl()
		{
		}
		
		void Update()
		{
			m_pSystem->Update();
		}

		int PlayClip(const std::string& fileName, const AudioSourceInfo& audioSourceInfo, Observer* observer)
		{
			return m_pSystem->PlayClip(fileName, audioSourceInfo, observer);
		}

		void SetMute(bool mute)
		{
			m_pSystem->SetMute(mute);
		}

		void ToggleMute()
		{
			m_pSystem->ToggleMute();
		}

		void SetMasterVolume(float volume)
		{
			m_pSystem->SetMasterVolume(volume);
		}

		bool IsMuted() const
		{
			return m_pSystem->IsMuted();
		}

		void LoadClip(const std::string& filePath)
		{
			m_pSystem->LoadClip(filePath);
		}

		float GetMasterVolume()
		{
			return m_pSystem->GetMasterVolume();
		}

		void SetVolume(const AudioSourceInfo& audioSourceInfo)
		{
			m_pSystem->SetVolume(audioSourceInfo);
		}

		float GetVolume(const AudioSourceInfo& audioSourceInfo)
		{
			return m_pSystem->GetVolume(audioSourceInfo);
		}

	private:
		std::unique_ptr<FmodSystem> m_pSystem{};

		const int m_MaxChannels{ 32 };		
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

int DDM::FmodSoundSystem::PlayClip(const AudioClip* clip, const AudioSourceInfo& audioSourceInfo, Observer* observer)
{
	return m_pImpl->PlayClip(clip->GetFilePath(), audioSourceInfo, observer);
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
	//m_pImpl->PauseAll();
}

void DDM::FmodSoundSystem::ResumeAll()
{
	//m_pImpl->ResumeAll();
}

void DDM::FmodSoundSystem::SetMasterVolume(float volume)
{
	m_pImpl->SetMasterVolume(volume);
}

float DDM::FmodSoundSystem::GetMasterVolume()
{
	return m_pImpl->GetMasterVolume();
}

void DDM::FmodSoundSystem::SetVolume(const AudioSourceInfo& audioSourceInfo)
{
	m_pImpl->SetVolume(audioSourceInfo);
}

float DDM::FmodSoundSystem::GetVolume(const AudioSourceInfo& audioSourceInfo)
{
	return m_pImpl->GetVolume(audioSourceInfo);
}
