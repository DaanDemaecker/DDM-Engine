// FmodSoundSystem.h
// This class will implement a soundsystem using FMOD

// Parent include
#include "ServiceLocator/SoundSystem/SoundSystem.h"

// Standard library include
#include <memory>

namespace DDM
{
	// Impl forward declaration
	class FmodImpl;

	class FmodSoundSystem : public SoundSystem
	{
	public:
		FmodSoundSystem();

		virtual void LoadClip(const AudioClip* clip) override;

		virtual int PlayClip(const AudioClip* clip, const AudioSourceInfo& audioSourceInfo, Observer* observer) override;

		virtual void ToggleMute() override;

		virtual bool IsMuted() const;

		virtual void SetMasterMute(bool mute) override;

		virtual void SetMute(const AudioSourceInfo& audioSourceInfo) override;

		virtual void Update() override;

		virtual void PauseAll() override;

		virtual void ResumeAll() override;

		virtual void SetMasterVolume(float volume) override;

		virtual float GetMasterVolume() override;

		virtual void SetVolume(const AudioSourceInfo& audioSourceInfo) override;

		virtual float GetVolume(const AudioSourceInfo& audioSourceInfo) override;

		virtual void UpdateSourceLocation(const AudioSourceInfo& audioSourceInfo, GameObject* pGameObject) override;

		virtual void UpdateListenerLocation(GameObject* pGameObject) override;
	private:
		std::unique_ptr<FmodImpl> m_pImpl;
	};
}