// AudioSourceInfo.h
// This file will hold a struct containing info aobut an audiosource
// This struct will be used in the initial playing for an audioclip

#ifndef _DDM_AUDIO_SOURCE_INFO_
#define _DDM_AUDIO_SOURCE_INFO_

namespace DDM
{
	struct AudioSourceInfo
	{
		int Channel{ -1 };
		float Volume{1.0};
		bool Muted{ false };
		bool Paused{ false };
	};
}

#endif // !_DDM_AUDIO_SOURCE_INFO_
