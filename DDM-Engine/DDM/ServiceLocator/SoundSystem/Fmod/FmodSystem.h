// FmodSystem.h
// This file will hold a wrapper for the FMOD::System class

#ifndef _DDM_FMOD_SYSTEM_
#define _DDM_FMOD_SYSTEM_

// File includes
#include "Includes/FmodIncludes.h"

// Standard library includes
#include <vector>
#include <unordered_map>
#include <iostream>

namespace DDM
{
	class FmodSystem final
	{
	public:
		FmodSystem() = delete;

		FmodSystem(int maxChannels);

		FmodSystem(FmodSystem&) = delete;
		FmodSystem(FmodSystem&&) = delete;

		FmodSystem& operator=(FmodSystem&) = delete;
		FmodSystem& operator=(FmodSystem&&) = delete;

		~FmodSystem();

		void Update();

		int PlayClip(const std::string& fileName);

		void SetMute(bool mute);

		void ToggleMute();

		void SetMasterVolume(float volume);

		bool IsMuted() const;

		void LoadClip(const std::string& filePath);

	private:
		// Max volume
		const float m_MaxVolume{ 1 };

		// Volume of sounds
		float m_MasterVolume{ 1 };

		// Indicates wether sound is muted
		bool m_IsMuted{ false };

		// Max amount of channels
		const int m_MaxChannels{};

		// FMOD core system
		FMOD::System* m_pSystem;

		// List of FMOD clips
		std::unordered_map<std::string, FMOD::Sound*> m_Clips{};

		// List of available channels
		std::vector<FMOD::Channel*> m_Channels{};

		/// <summary>
		/// Get a channel index that isn't playing
		/// </summary>
		/// <returns>Channel index</returns>
		int GetFreeChannel();

		/// <summary>
		/// Create a single audioclip
		/// </summary>
		/// <param name="fileName: ">path to audio file</param>
		void CreateClip(const std::string& fileName);

		void SetVolume();
	};
}

#endif // !_DDM_FMOD_SYSTEM_
