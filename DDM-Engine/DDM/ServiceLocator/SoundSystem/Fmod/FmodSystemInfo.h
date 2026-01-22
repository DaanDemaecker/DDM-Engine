// FmodSystemInfo.h
// This file will hold a struct containing info aobut the fmod system

#ifndef _DDM_FMOD_SYSTEM_INFO_
#define _DDM_FMOD_SYSTEM_INFO_

namespace DDM
{
	struct FmodSystemInfo
	{
		float Volume{ 1.0 };
		bool Muted{ false };
		bool Paused{ false };
		bool Is3D{ true };
	};
}

#endif // !_DDM_FMOD_SYSTEM_INFO_
