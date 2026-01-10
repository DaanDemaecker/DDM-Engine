// Subject.cpp

// Header include
#include "Subject.h"

// File includes
#include "Observer.h"

// Standard library includes
#include <algorithm>

void DDM::Subject::NotifyObservers(const Event& event)
{
	for (auto observer : m_pObservers)
	{
		observer->Notify(event);
	}
}

void DDM::Subject::AddObserver(Observer* observer)
{
	if (std::find(m_pObservers.begin(), m_pObservers.end(), observer) != m_pObservers.end())
	{
		m_pObservers.push_back(observer);
	}
}

void DDM::Subject::RemoveObserver(const Observer* observer)
{
	m_pObservers.erase(std::remove(m_pObservers.begin(), m_pObservers.end(), observer), m_pObservers.end());
}
