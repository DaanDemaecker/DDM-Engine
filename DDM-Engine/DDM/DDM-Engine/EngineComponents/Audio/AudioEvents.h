// AudioEvents.h
// This file will define events concerning audio

#ifndef _DDM_AUDIO_EVENTS_
#define _DDM_AUDIO_EVENTS_

// File includes
#include "DDM-Engine/Events/Event.h"

namespace DDM
{
	struct AudioFinishedEvent : public Event
	{
		AudioFinishedEvent(int index)
		{
			Index = index;
		}

		int Index;
	};
}

#endif // !_DDM_AUDIO_EVENTS_