// Event.h
// This file will hold the base struct of all events

#ifndef _DDM_EVENT_
#define _DDM_EVENT_

namespace DDM
{
	struct Event
	{
		Event() = default;
		virtual ~Event() = default;
	};
}

#endif // !_DDM_EVENT_
