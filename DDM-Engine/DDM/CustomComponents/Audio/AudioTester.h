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
		/// Used to render GUI
		/// </summary>
		virtual void OnGUI() override;
	private:
		// Max length for text input
		const int m_TextLength{ 125 };

		// String for filepath to clip to play
		std::vector<char> m_ClipPath{};

		// String for filepath to stream to play
		std::vector<char> m_StreamPath{};

		/// <summary>
		/// Play an audio clip given a filepath
		/// </summary>
		/// <param name="filePath: ">Filepath to the audioclip</param>
		/// <param name="bufferLength: ">Length of the text buffer</param>
		void PlayCLip(char* filePath, int bufferLength);

		/// <summary>
		/// Play an audio stream given a filepath
		/// </summary>
		/// <param name="filePath: ">Filepath to the audioclip</param>
		/// <param name="bufferLength: ">Length of the text buffer</param>
		void PlayStream(char* filePath, int bufferLength);

	};
}

#endif // !_DDM_AUDIO_TESTER_
