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

		void SetMasterMute(bool mute)
		{
			m_pSystem->SetMasterMute(mute);
		}

		void SetMute(const AudioSourceInfo& info)
		{
			m_pSystem->SetMute(info);
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

		void UpdateSourceLocation(const AudioSourceInfo& audioSourceInfo, GameObject* pGameObject)
		{
			m_pSystem->UpdateSourceLocation(audioSourceInfo, pGameObject);
		}

		void UpdateListenerLocation(GameObject* pGameObject)
		{
			m_pSystem->UpdateListenerLocation(pGameObject);
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

void DDM::FmodSoundSystem::SetMasterMute(bool mute)
{
	m_pImpl->SetMasterMute(mute);
}

void DDM::FmodSoundSystem::SetMute(const AudioSourceInfo& audioSourceInfo)
{
	m_pImpl->SetMute(audioSourceInfo);
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

void DDM::FmodSoundSystem::UpdateSourceLocation(const AudioSourceInfo& audioSourceInfo, GameObject* pGameObject)
{
	m_pImpl->UpdateSourceLocation(audioSourceInfo, pGameObject);
}

void DDM::FmodSoundSystem::UpdateListenerLocation(GameObject* pGameObject)
{
	m_pImpl->UpdateListenerLocation(pGameObject);
}
