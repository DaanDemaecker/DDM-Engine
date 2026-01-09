// SoundSystem.h
// This file will contain the base class and default class for the soundsystem

#ifndef _DDM_SOUND_SYSTEM_
#define _DDM_SOUND_SYSTEM_

// File includes
#include "EngineComponents/Audio/AudioClip.h"

// Standard library includes
#include <string>

namespace DDM
{
	class SoundSystem
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		SoundSystem() = default;

		/// <summary>
		/// Default destructor
		/// </summary>
		virtual ~SoundSystem() = default;

		// Delete copy and move operations
		SoundSystem(SoundSystem& other) = delete;
		SoundSystem(SoundSystem&& other) = delete;

		SoundSystem& operator=(SoundSystem& other) = delete;
		SoundSystem& operator=(SoundSystem&& other) = delete;

		/// <summary>
		/// Load in a clip before playing it
		/// </summary>
		/// <param name="clip: ">reference to the audioclip</param>
		virtual void LoadClip(const AudioClip* clip) = 0;

		/// <summary>
		/// Play an audio clip
		/// </summary>
		/// <param name="clip: ">reference to the audioclip</param>
		virtual int PlayClip(const AudioClip* clip) = 0;

		/// <summary>
		/// Query wether sound system is muted
		/// </summary>
		/// <returns: >bool indicating if soundsystem is muted or not</returns>
		virtual bool IsMuted() const = 0;

		/// <summary>
		/// Toggle wether the sound is muted or not
		/// </summary>
		virtual void ToggleMute() = 0;

		/// <summary>
		/// Set wether the sound is muted or not
		/// </summary>
		/// <param name="mute: ">new value</param>
		virtual void SetMute(bool mute) = 0;

		/// <summary>
		/// Update the sound system
		/// </summary>
		virtual void Update() = 0;

		/// <summary>
		/// Pause all channels
		/// </summary>
		virtual void PauseAll() = 0;

		/// <summary>
		/// Resume all channels
		/// </summary>
		virtual void ResumeAll() = 0;

		/// <summary>
		/// Set master volume to a new volume between 0 and 1
		/// </summary>
		/// <param name="volume: ">new volume</param>
		virtual void SetMasterVolume(float volume) = 0;
	};

	class DefaultSoundSystem final : public SoundSystem
	{
	public:
		DefaultSoundSystem() = default;
		virtual ~DefaultSoundSystem() override = default;

		virtual void LoadClip(const AudioClip* clip) override {}

		virtual int PlayClip(const AudioClip* clip) override { return 0; }

		virtual bool IsMuted() const override { return false; }

		virtual void ToggleMute() override {}

		virtual void SetMute(bool mute) override {}

		virtual void Update() override {}

		virtual void PauseAll() override {}

		virtual void ResumeAll() override {}

		virtual void SetMasterVolume(float volume) override {}
	};
}

#endif // !_DDM_SOUND_SYSTEM_
