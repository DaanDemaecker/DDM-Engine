#ifndef SingletonIncluded
#define SingletonIncluded

// Export include
#include "DDM-Engine/Export.h"

namespace DDM
{
	template <typename T>
	class DDM_API Singleton
	{
	public:
		static T& GetInstance()
		{
			static T instance{};
			return instance;
		}

		virtual ~Singleton() = default;
		Singleton(const Singleton& other) = delete;
		Singleton(Singleton&& other) = delete;
		Singleton& operator=(const Singleton& other) = delete;
		Singleton& operator=(Singleton&& other) = delete;

	protected:
		Singleton() = default;
	};
}

#endif // !SingletonIncluded