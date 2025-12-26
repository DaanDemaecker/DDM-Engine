// ServiceLocator.h
// This class will serve as a locator for services like the soundsystem

#ifndef _DDM_SERVICE_LOCATOR_
#define _DDM_SERVICE_LOCATOR_

namespace DDM
{
	class ServiceLocator
	{
	public:
		ServiceLocator() = default;
		~ServiceLocator() = default;

		ServiceLocator(ServiceLocator&) = delete;
		ServiceLocator(ServiceLocator&&) = delete;

		ServiceLocator& operator=(ServiceLocator&) = delete;
		ServiceLocator& operator=(ServiceLocator&&) = delete;

	private:

	};
}

#endif // !_DDM_SERVICE_LOCATOR_
