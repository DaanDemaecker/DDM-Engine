// Subject.h
// This file will hold the baseclass for a subject object

#ifndef _DDM_SUBJECT_
#define _DDM_SUBJECT_

// File includes
#include "Event.h"

// Export include
#include "DDM-Engine/Export.h"

// Standard library includes
#include <vector>

namespace DDM
{
	class Observer;

	class DDM_API Subject
	{
	public:
		Subject() = default;
		virtual ~Subject() = default;

		/// <summary>
		/// Send a notification to all observers
		/// </summary>
		/// <param name="event: ">extra data</param>
		void NotifyObservers(const Event&& event);

		/// <summary>
		/// Send a notification to all observers
		/// </summary>
		/// <param name="event: ">extra data</param>
		void NotifyObservers(const Event& event);

		/// <summary>
		/// Add a single observer to the list
		/// </summary>
		/// <param name="observer: ">observer to add</param>
		void AddObserver(Observer* observer);

		/// <summary>
		/// Remove a single observer
		/// </summary>
		/// <param name="observer: ">observer to remove</param>
		void RemoveObserver(const Observer* observer);

	private:
		std::vector<Observer*> m_pObservers{};
	};
}

#endif // !_DDM_SUBJECT_
