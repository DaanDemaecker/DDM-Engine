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

		virtual void PlayClip(const AudioClip* clip) override;

		virtual void ToggleMute() override;

		virtual bool IsMuted() const;

		virtual void SetMute(bool mute) override;

		virtual void Update() override;

		virtual void PauseAll() override;

		virtual void ResumeAll() override;

		virtual void SetMasterVolume(float volume) override;

	private:
		std::unique_ptr<FmodImpl> m_pImpl;
	};
}