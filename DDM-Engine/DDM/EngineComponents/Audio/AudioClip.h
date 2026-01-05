// AudioClip.h
// This file will hold the base class of a single audioclip

#ifndef _DDM_AUDIO_CLIP_
#define _DDM_AUDIO_CLIP_

// Standard library includes
#include <string>

namespace DDM
{
	struct AudioClip
	{
		// Delete default constructor
		AudioClip() = delete;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path: ">filepath to the audio file</param>
		AudioClip(std::string& path)
			:filePath(path)
		{

		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path: ">filepath to the audio file</param>
		AudioClip(std::string&& path)
			:AudioClip(path)
		{

		}

		// Path to the audio file
		const std::string filePath;
	};
}

#endif // !_DDM_AUDIO_CLIP_
