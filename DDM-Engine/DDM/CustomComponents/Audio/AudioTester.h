// AudioTester.h
// This component will provide a gui that can be used to test audio

#ifndef _DDM_AUDIO_TESTER_
#define _DDM_AUDIO_TESTER_

// Parent include
#include "BaseClasses/Component.h"

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
		

	};
}

#endif // !_DDM_AUDIO_TESTER_
