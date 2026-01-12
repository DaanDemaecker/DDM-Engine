// AudioTester.h
// This component will provide a gui that can be used to test audio

#ifndef _DDM_AUDIO_TESTER_
#define _DDM_AUDIO_TESTER_

// Parent include
#include "BaseClasses/Component.h"

// Standard library includes
#include <iostream>

namespace DDM
{
	// Class forward declaration
	class AudioSource;

	class AudioTester final : public Component
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		AudioTester();

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~AudioTester();
		
		/// <summary>
		/// Called when a new scene is loaded in
		/// </summary>
		virtual void OnSceneLoad() override;

		/// <summary>
		/// Used to render GUI
		/// </summary>
		virtual void OnGUI() override;
	private:
		// Master volume
		float m_Volume{ 1 };

		bool m_IsMuted{ false };

		// Max length for text input
		const int m_TextLength{ 125 };

		// String for filepath to clip to play
		std::vector<char> m_ClipPath{};

		// Audiosource attached to this gameobject
		std::shared_ptr<AudioSource> m_pAudioSource{};

		/// <summary>
		/// Play an audio clip given a filepath
		/// </summary>
		/// <param name="filePath: ">Filepath to the audioclip</param>
		/// <param name="bufferLength: ">Length of the text buffer</param>
		void SetCLip(char* filePath, int bufferLength);
	};
}

#endif // !_DDM_AUDIO_TESTER_
