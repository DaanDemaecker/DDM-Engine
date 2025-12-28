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

		virtual void Play(std::string& fileName) override;

		virtual void ToggleMute() override {};

		virtual void SetMute(bool mute) override {};

		virtual void SetVolume(float volume) override {};

		virtual void Update();

	private:
		std::unique_ptr<FmodImpl> m_pImpl;
	};
}