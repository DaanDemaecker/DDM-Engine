// AudioSource.h
// This file will represent the AudioSource component
// This component will be able to play audioclips and hold settings for the audioclip

#ifndef _DDM_AUDIO_SOURCE_
#define _DDM_AUDIO_SOURCE_

// Parent include
#include "BaseClasses/Component.h"
#include "Events/Observer.h"

// File includes
#include "AudioClip.h"
#include "AudioSourceInfo.h"

namespace DDM
{
	class AudioSource final : public Component, public Observer
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		AudioSource();

		/// <summary>
		/// Default destructor
		/// </summary>
		~AudioSource() = default;

		// Rule of five
		AudioSource(const AudioSource& other) = delete;
		AudioSource(AudioSource&& other) = delete;

		AudioSource& operator=(const AudioSource& other) = delete;
		AudioSource& operator=(AudioSource&& other) = delete;

		/// <summary>
		/// Used to render GUI
		/// </summary>
		virtual void OnGUI() override;

		/// <summary>
		/// Gets called at the end of the update loop
		/// </summary>
		virtual void LateUpdate() override;

		/// <summary>
		/// Set the audio clip
		/// </summary>
		/// <param name="clip: ">reference to audio clip</param>
		void SetClip(const std::shared_ptr<AudioClip> clip);
		
		/// <summary>
		/// Set the audio clip
		/// </summary>
		/// <param name="path: ">path to the audio file</param>
		void SetClip(const std::string& path);

		/// <summary>
		/// Set the audio clip
		/// </summary>
		/// <param name="path: ">path to the audio file</param>
		void SetClip(const std::string&& path);

		/// <summary>
		/// Get the audioclip
		/// </summary>
		/// <returns>Shared pointer to audioclip</returns>
		std::shared_ptr<AudioClip> GetClip() const;

		/// <summary>
		/// Play the currently selected clip
		/// </summary>
		void Play();

		/// <summary>
		/// Set the volume
		/// </summary>
		/// <param name="volume: ">value between 0 and 1</param>
		void SetVolume(float volume);

		/// <summary>
		/// Get the current volume
		/// </summary>
		/// <returns>Value between 0 and 1 indicating volume</returns>
		float GetVolume() const { return m_Info.Volume; }

		/// <summary>
		/// Set whether audiosource is muted or not
		/// </summary>
		/// <param name="mute"></param>
		void SetMute(bool mute);

		/// <summary>
		/// Get whether audiosource is muted or not
		/// </summary>
		/// <returns>Value indicating muted value</returns>
		bool GetMute() const { return m_Info.Muted; }

		/// <summary>
		/// Set paused mode
		/// </summary>
		/// <param name="paused: ">new paused mode</param>
		void SetPaused(bool paused);

		/// <summary>
		/// Get whether audiosource is paused or not
		/// </summary>
		/// <returns>Bool indicating paused value</returns>
		bool GetPaused() const { return m_Info.Paused; }

		/// <summary>
		/// Set 3D mode
		/// </summary>
		/// <param name="is3D: ">new 3D mode</param>
		void Set3D(bool is3D);

		/// <summary>
		/// Get whether audiosource is 3D or not
		/// </summary>
		/// <returns>Bool indicating 3D value</returns>
		bool Get3D() const { return m_Info.Is3D; }

		/// <summary>
		/// Stop the audio currently playing
		/// </summary>
		void Stop();

		/// <summary>
		/// Set new looping mode
		/// </summary>
		/// <param name="looping: ">new looping mode</param>
		void SetLooping(bool looping);

		/// <summary>
		/// Get whether audiosource is looping
		/// </summary>
		/// <returns>Bool indiciating looping value</returns>
		bool GetLooping() const { return m_Info.Looping; }

		/// <summary>
		/// Set the frequency of the audiosource
		/// </summary>
		/// <param name="frequency: ">new frequency</param>
		void SetFrequency(float frequency);

		/// <summary>
		/// Get the current frequency
		/// </summary>
		/// <returns>Float indicating frequency</returns>
		float GetFrequency() const { return m_Info.Frequency; }

		/// <summary>
		/// Set the priority value of this audiosource
		/// </summary>
		/// <param name="priority: ">new priority value</param>
		void SetPriority(int priority);

		/// <summary>
		/// Receive a notification from the subject
		/// </summary>
		/// <param name="event: ">event that triggered the notification</param>
		virtual void Notify(const Event& event);
	private:
		// The currently set audioclip
		std::shared_ptr<AudioClip> m_pClip{};

		// Info about the audiosource
		AudioSourceInfo m_Info{};
	};
}

#endif // !_DDM_AUDIO_SOURCE_
