// FmodSystem.h
// This file will hold a wrapper for the FMOD::System class

#ifndef _DDM_FMOD_SYSTEM_
#define _DDM_FMOD_SYSTEM_

// File includes
#include "Includes/FmodIncludes.h"
#include "Events/Observer.h"
#include "FmodChannel.h"

// Standard library includes
#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>

namespace DDM
{
	struct AudioSourceInfo;
	class GameObject;

	class FmodSystem final : public Observer
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

		int PlayClip(const std::string& fileName, const AudioSourceInfo& audioSourceInfo, Observer* observer);

		void SetMasterMute(bool mute);

		void ToggleMute();

		void SetMasterVolume(float volume);

		float GetMasterVolume() const;

		bool IsMuted() const;

		void LoadClip(const std::string& filePath);

		void SetVolume(const AudioSourceInfo& audioSourceInfo);

		float GetVolume(const AudioSourceInfo& audioSourceInfo);

		/// <summary>
		/// Receive a notification from the subject
		/// </summary>
		/// <param name="event: ">event that triggered the notification</param>
		virtual void Notify(const Event& event);

		void SetMute(const AudioSourceInfo& info);

		void UpdateSourceLocation(const AudioSourceInfo& audioSourceInfo, GameObject* pGameObject);

		void UpdateListenerLocation(GameObject* pGameObject);
	private:
		// Max volume
		const float m_MaxVolume{ 1 };

		// Volume of sounds
		float m_MasterVolume{ 1 };

		// Indicates wether sound is muted
		bool m_MasterMute{ false };

		// Max amount of channels
		const int m_MaxChannels{};

		// FMOD core system
		FMOD::System* m_pSystem;

		// List of FMOD clips
		std::unordered_map<std::string, FMOD::Sound*> m_Clips{};

		// List of available channels
		std::vector<std::unique_ptr<FmodChannel>> m_Channels{};

		// Channels to remove in next update
		std::vector<int> m_ChannelsToRemove{};

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
	};
}

#endif // !_DDM_FMOD_SYSTEM_
