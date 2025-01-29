#ifndef D3DEngineIncluded
#define D3DEngineIncluded

// File includes
#include "Includes/GLFWIncludes.h"

// Standard library includes
#include <string>
#include <functional>

namespace DDM3
{
	class DDM3Engine
	{
	public:
		DDM3Engine();
		~DDM3Engine();
		void Run(const std::function<void()>& load);

		DDM3Engine(const DDM3Engine& other) = delete;
		DDM3Engine(DDM3Engine&& other) = delete;
		DDM3Engine& operator=(const DDM3Engine& other) = delete;
		DDM3Engine& operator=(DDM3Engine&& other) = delete;
	};
}

#endif // !D3DEngineIncluded