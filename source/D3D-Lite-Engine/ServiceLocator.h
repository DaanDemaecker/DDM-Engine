// ServiceLocator.h
// This class will hold and manage services like soundsystems and model loaders

#ifndef ServiceLocatorIncluded
#define ServiceLocatorIncluded

namespace D3D
{
	class ServiceLocator final
	{
	public:
		ServiceLocator() = default;
		~ServiceLocator() = default;

		ServiceLocator(ServiceLocator& other) = delete;
		ServiceLocator(ServiceLocator&& other) = delete;
		ServiceLocator operator=(ServiceLocator& other) = delete;
		ServiceLocator operator=(ServiceLocator&& other) = delete;

	private:

	};
}
#endif // !ServiceLocatorIncluded