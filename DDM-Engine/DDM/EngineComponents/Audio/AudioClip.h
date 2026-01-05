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
		/// <summary>
		/// Default constructor
		/// </summary>
		AudioClip();

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
		/// Copy constructor
		/// </summary>
		/// <param name="other: ">audioclip to copy</param>
		AudioClip(const AudioClip& other);

		/// <summary>
		/// Move constructor
		/// </summary>
		/// <param name="other: ">audioclip to move</param>
		AudioClip(const AudioClip&& other) noexcept;

		/// <summary>
		/// Copy asignment operator
		/// </summary>
		/// <param name="other: ">audioclip to copy</param>
		/// <returns>new audioclip</returns>
		AudioClip& operator=(const AudioClip& other);

		/// <summary>
		/// Copy asignment operator
		/// </summary>
		/// <param name="other: ">audioclip to move</param>
		/// <returns>new audioclip</returns>
		AudioClip& operator=(const AudioClip&& other) noexcept;

		/// <summary>
		/// Get the saved file path
		/// </summary>
		/// <returns>File path</returns>
		const std::string& GetFilePath() const { return m_FilePath; }

	private:
		// Path to the audio file
		std::string m_FilePath{};
	};
}

#endif // !_DDM_AUDIO_CLIP_
