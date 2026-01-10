// FmodChannel.h
// This class will serve as a wrapper for the Fmod::Channel class

#ifndef _DDM_FMOD_CHANNEL_
#define _DDM_FMOD_CHANNEL_

// File includes
#include "Includes/FmodIncludes.h"

namespace DDM
{
	class FmodChannel final
	{
	public:
		FmodChannel() = delete;

		FmodChannel(FMOD::Channel* pChannel);

	private:
		FMOD::Channel* m_pChannel{};

		static FMOD_RESULT F_CALLBACK ChannelCallback(
			FMOD_CHANNELCONTROL* channelcontrol,
			FMOD_CHANNELCONTROL_TYPE controltype,
			FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
			void* commanddata1,
			void* commanddata2
		);
	};
}

#endif // !_DDM_FMOD_CHANNEL_
