#ifndef D3DEngineIncluded
#define D3DEngineIncluded
#include "GLFWIncludes.h"

#include <string>
#include <functional>

namespace D3D
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

	private:
	};
}

#endif // !D3DEngineIncluded