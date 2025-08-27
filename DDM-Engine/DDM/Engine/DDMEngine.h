#ifndef D3DEngineIncluded
#define D3DEngineIncluded

// File includes
#include "Includes/GLFWIncludes.h"
#include "Vulkan/VulkanObject.h"
#include "Engine/Window.h"

// Standard library includes
#include <string>
#include <functional>

namespace DDM
{
	class DDMEngine
	{
	public:
		DDMEngine();
		~DDMEngine();

		template <class T>
		void Init();

		void Run(const std::function<void()>& load);

		DDMEngine(const DDMEngine& other) = delete;
		DDMEngine(DDMEngine&& other) = delete;
		DDMEngine& operator=(const DDMEngine& other) = delete;
		DDMEngine& operator=(DDMEngine&& other) = delete;
	
	private:
		bool m_Initialized = false;
	};

	template <class T>
	void DDMEngine::Init()
	{
		// Create the window with the given width and height
		DDM::Window::GetInstance();

		auto& vulkanObject = VulkanObject::GetInstance();

		vulkanObject.Init<T>();

		m_Initialized = true;
	}

}

#endif // !D3DEngineIncluded