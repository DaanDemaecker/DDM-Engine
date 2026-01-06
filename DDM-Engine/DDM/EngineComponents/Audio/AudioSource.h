// AudioSource.h
// This file will represent the AudioSource component
// This component will be able to play audioclips and hold settings for the audioclip

#ifndef _DDM_AUDIO_SOURCE_
#define _DDM_AUDIO_SOURCE_

// Parent include
#include "BaseClasses/Component.h"

// File includes
#include "AudioClip.h"

namespace DDM
{
	class AudioSource final : public Component
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
		/// Get the currently set clip
		/// </summary>
		/// <returns></returns>
		const AudioClip* GetClip() const { return m_pClip.get(); }

		/// <summary>
		/// Play the currently selected clip
		/// </summary>
		void Play();
	private:
		// The currently set audioclip
		std::shared_ptr<AudioClip> m_pClip{};

	};
}

#endif // !_DDM_AUDIO_SOURCE_
