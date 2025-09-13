// DDMEngine.h
// This class will serve as the core engine, managing the update loop and initialization of certain objects

#ifndef _DDM_DDM_ENGINE_
#define _DDM_DDM_ENGINE_

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
		/// <summary>
		/// Constructor
		/// </summary>
		DDMEngine();
		
		/// <summary>
		/// Destructor
		/// </summary>
		~DDMEngine();

		/// <summary>
		/// Initalizes the engine
		/// </summary>
		/// <typeparam name="T: ">Type of renderer to use</typeparam>
		template <class T>
		void Init();

		/// <summary>
		/// Runs the engine
		/// </summary>
		/// <param name="load: ">Reference to the sceneloader function</param>
		void Run(const std::function<void()>& load);

		// Rule of 5
		DDMEngine(const DDMEngine& other) = delete;
		DDMEngine(DDMEngine&& other) = delete;
		DDMEngine& operator=(const DDMEngine& other) = delete;
		DDMEngine& operator=(DDMEngine&& other) = delete;
	
	private:
		// Indicates wether engine is initialized
		bool m_Initialized = false;
	};

	template <class T>
	void DDMEngine::Init()
	{
		// Create the window with the given width and height
		DDM::Window::GetInstance();

		// Get a reference to the vulkan object and initialize
		auto& vulkanObject = VulkanObject::GetInstance();
		vulkanObject.Init<T>();

		// Indicate that engine is initialized
		m_Initialized = true;
	}

}

#endif // !_DDM_DDM_ENGINE_