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

		virtual int PlayClip(const AudioClip* clip, const AudioSourceInfo& info, Observer* observer) override;

		virtual void ToggleMute() override;

		virtual bool IsMuted() const;

		virtual void SetMasterMute(bool mute) override;

		virtual void SetMute(const AudioSourceInfo& info) override;

		virtual void Update() override;

		virtual void SetMasterVolume(float volume) override;

		virtual float GetMasterVolume() override;

		virtual void SetVolume(const AudioSourceInfo& info) override;

		virtual float GetVolume(const AudioSourceInfo& info) override;

		virtual void UpdateSourceLocation(const AudioSourceInfo& info, GameObject* pGameObject) override;

		virtual void UpdateListenerLocation(GameObject* pGameObject) override;

		virtual void SetPaused(const AudioSourceInfo& info) override;

		virtual void Set3D(const AudioSourceInfo& info) override;

		virtual void SetMaster3D(bool is3D) override;

		virtual bool GetMaster3D() override;

		virtual void SetMasterPaused(bool paused) override {};

		virtual bool GetMasterPaused() const override { return false; }

		virtual void Stop(AudioSourceInfo& info) override;

		virtual void StopAll() override;

		virtual void SetLoop(AudioSourceInfo& sourceInfo) override;

		virtual void SetFrequency(AudioSourceInfo& sourceInfo) override;

		virtual void SetMasterFrequency(float frequency) override;

		virtual float GetMasterFrequency() override;

		virtual void SetPriority(AudioSourceInfo& sourceInfo) override;
	private:
		std::unique_ptr<FmodImpl> m_pImpl;
	};
}