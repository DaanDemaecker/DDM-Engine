// AudioClip.h
// This file will hold the base class of a single audioclip

#ifndef _DDM_AUDIO_CLIP_
#define _DDM_AUDIO_CLIP_

// Standard library includes
#include <string>

namespace DDM
{
	class AudioClip
	{
	public:
		// Delete default constructor
		AudioClip() = delete;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path: ">filepath to the audio file</param>
		AudioClip(const std::string& path);

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path: ">filepath to the audio file</param>
		AudioClip(const std::string&& path);

		/// <summary>
		/// Get the saved file path
		/// </summary>
		/// <returns>File path</returns>
		const std::string& GetFilePath() const { return m_FilePath; }

	private:
		// Path to the audio file
		const std::string m_FilePath{};
	};
}

#endif // !_DDM_AUDIO_CLIP_
