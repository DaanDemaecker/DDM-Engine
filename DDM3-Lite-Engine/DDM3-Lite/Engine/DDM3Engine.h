#ifndef D3DEngineIncluded
#define D3DEngineIncluded

// File includes
#include "Includes/GLFWIncludes.h"
#include "Vulkan/VulkanObject.h"
#include "Engine/Window.h"

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

		template <class T>
		void Init();

		void Run(const std::function<void()>& load);

		DDM3Engine(const DDM3Engine& other) = delete;
		DDM3Engine(DDM3Engine&& other) = delete;
		DDM3Engine& operator=(const DDM3Engine& other) = delete;
		DDM3Engine& operator=(DDM3Engine&& other) = delete;
	
	private:
		bool m_Initialized = false;
	};

	template <class T>
	void DDM3Engine::Init()
	{
		// Create the window with the given width and height
		DDM3::Window::GetInstance();

		auto& vulkanObject = VulkanObject::GetInstance();

		vulkanObject.Init<T>();

		m_Initialized = true;
	}

}

#endif // !D3DEngineIncluded