#ifndef D3DEngineIncluded
#define D3DEngineIncluded

// File includes
#include "Includes/GLFWIncludes.h"

// Standard library includes
#include <string>
#include <functional>

namespace DDM3
{
	class D3DEngine
	{
	public:
		D3DEngine();
		~D3DEngine();
		void Run(const std::function<void()>& load);

		D3DEngine(const D3DEngine& other) = delete;
		D3DEngine(D3DEngine&& other) = delete;
		D3DEngine& operator=(const D3DEngine& other) = delete;
		D3DEngine& operator=(D3DEngine&& other) = delete;
	};
}

#endif // !D3DEngineIncluded