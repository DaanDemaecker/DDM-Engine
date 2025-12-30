// SoundSystem.h
// This file will contain the base class and default class for the soundsystem

#ifndef _DDM_SOUND_SYSTEM_
#define _DDM_SOUND_SYSTEM_

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
		/// Play a sound clip given a filepath
		/// </summary>
		/// <param name="fileName: ">path to the requested clip</param>
		virtual void PlayClip(std::string&& fileName)
		{
			// Forward to l-value overloaded function
			PlayClip(fileName);
		}

		/// <summary>
		/// Play a sound clip given a filepath
		/// </summary>
		/// <param name="fileName: ">path to the requested clip</param>
		virtual void PlayClip(std::string& fileName) = 0;

		/// <summary>
		/// Play a sound track given a filepath
		/// </summary>
		/// <param name="fileName: ">path to the requested track</param>
		virtual void PlayStream(std::string&& fileName)
		{
			// Forward to l-value overloaded function
			PlayStream(fileName);
		}

		/// <summary>
		/// Play a sound track given a filepath
		/// </summary>
		/// <param name="fileName: ">path to the requested track</param>
		virtual void PlayStream(std::string& fileName) = 0;

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
		/// Set the master volume of all sound
		/// </summary>
		/// <param name="volume: ">value of the new folume</param>
		virtual void SetVolume(float volume) = 0;

		/// <summary>
		/// Update the sound system
		/// </summary>
		virtual void Update() = 0;

	};

	class DefaultSoundSystem final : public SoundSystem
	{
	public:
		DefaultSoundSystem() = default;
		virtual ~DefaultSoundSystem() override = default;

		virtual void PlayClip(std::string& fileName) override {}

		virtual void PlayStream(std::string& fileName) override {}

		virtual void ToggleMute() override {}

		virtual void SetMute(bool mute) override {}

		virtual void SetVolume(float volume) override {}

		virtual void Update() override {}
	};
}

#endif // !_DDM_SOUND_SYSTEM_
