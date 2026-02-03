// AudioListener.h
// This component will serve as a listener for audio

#ifndef _DDM_AUDIO_LISTENER_
#define _DDM_AUDIO_LISTENER_

// Parent include
#include "DDM-Engine/BaseClasses/Component.h"

namespace DDM
{
	class AudioListener final : public Component
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		AudioListener();

		/// <summary>
		/// Default destructor
		/// </summary>
		~AudioListener() = default;

		// Rule of five
		AudioListener(const AudioListener& other) = delete;
		AudioListener(AudioListener&& other) = delete;

		AudioListener& operator=(const AudioListener& other) = delete;
		AudioListener& operator=(AudioListener&& other) = delete;

		virtual void LateUpdate() override;
	private:
	};
}

#endif // !_DDM_AUDIO_LISTENER_
