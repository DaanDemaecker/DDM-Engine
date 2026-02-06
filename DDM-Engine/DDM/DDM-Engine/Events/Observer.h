// Observer.h
// This file will hold the base observer class

#ifndef _DDM_OBSERVER_
#define _DDM_OBSERVER_

// File includes
#include "Event.h"

// Export include
#include "DDM-Engine/Export.h"

namespace DDM
{
	class DDM_API Observer
	{
	public:
		/// <summary>
		/// Receive a notification from the subject
		/// </summary>
		/// <param name="event: ">event that triggered the notification</param>
		virtual void Notify(const Event& event) = 0;
	};
}

#endif // !_DDM_OBSERVER_
