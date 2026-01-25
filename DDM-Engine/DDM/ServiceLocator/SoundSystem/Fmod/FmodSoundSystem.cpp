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

		int PlayClip(const std::string& fileName, const AudioSourceInfo& info, Observer* observer)
		{
			return m_pSystem->PlayClip(fileName, info, observer);
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

		void SetVolume(const AudioSourceInfo& info)
		{
			m_pSystem->SetVolume(info);
		}

		float GetVolume(const AudioSourceInfo& info)
		{
			return m_pSystem->GetVolume(info);
		}

		void UpdateSourceLocation(const AudioSourceInfo& info, GameObject* pGameObject)
		{
			m_pSystem->UpdateSourceLocation(info, pGameObject);
		}

		void UpdateListenerLocation(GameObject* pGameObject)
		{
			m_pSystem->UpdateListenerLocation(pGameObject);
		}

		void SetPaused(const AudioSourceInfo& info)
		{
			m_pSystem->SetPaused(info);
		}

		void Set3D(const AudioSourceInfo& info)
		{
			m_pSystem->Set3D(info);
		}

		void SetMaster3D(bool is3D)
		{
			m_pSystem->SetMaster3D(is3D);
		}

		bool GetMaster3D()
		{
			return m_pSystem->GetMaster3D();
		}


		void Stop(AudioSourceInfo& info)
		{
			m_pSystem->Stop(info);
		}

		void StopAll()
		{
			m_pSystem->StopAll();
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

int DDM::FmodSoundSystem::PlayClip(const AudioClip* clip, const AudioSourceInfo& info, Observer* observer)
{
	return m_pImpl->PlayClip(clip->GetFilePath(), info, observer);
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

void DDM::FmodSoundSystem::SetMute(const AudioSourceInfo& info)
{
	m_pImpl->SetMute(info);
}

void DDM::FmodSoundSystem::Update()
{
	m_pImpl->Update();
}

void DDM::FmodSoundSystem::SetMasterVolume(float volume)
{
	m_pImpl->SetMasterVolume(volume);
}

float DDM::FmodSoundSystem::GetMasterVolume()
{
	return m_pImpl->GetMasterVolume();
}

void DDM::FmodSoundSystem::SetVolume(const AudioSourceInfo& info)
{
	m_pImpl->SetVolume(info);
}

float DDM::FmodSoundSystem::GetVolume(const AudioSourceInfo& info)
{
	return m_pImpl->GetVolume(info);
}

void DDM::FmodSoundSystem::UpdateSourceLocation(const AudioSourceInfo& info, GameObject* pGameObject)
{
	m_pImpl->UpdateSourceLocation(info, pGameObject);
}

void DDM::FmodSoundSystem::UpdateListenerLocation(GameObject* pGameObject)
{
	m_pImpl->UpdateListenerLocation(pGameObject);
}

void DDM::FmodSoundSystem::SetPaused(const AudioSourceInfo& info)
{
	m_pImpl->SetPaused(info);
}

void DDM::FmodSoundSystem::Set3D(const AudioSourceInfo& info)
{
	m_pImpl->Set3D(info);
}

void DDM::FmodSoundSystem::SetMaster3D(bool is3D)
{
	m_pImpl->SetMaster3D(is3D);
}

bool DDM::FmodSoundSystem::GetMaster3D()
{
	return m_pImpl->GetMaster3D();
}

void DDM::FmodSoundSystem::Stop(AudioSourceInfo& info)
{
	m_pImpl->Stop(info);
}

void DDM::FmodSoundSystem::StopAll()
{
	m_pImpl->StopAll();
}
