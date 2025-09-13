// D3DEngine.cpp

// Header include
#include "DDMEngine.h"

// File includes
#include "Scene.h"
#include "Window.h"


#include "Managers/SceneManager.h"
#include "Managers/ConfigManager.h"
#include "Managers/TimeManager.h"


#include "Vulkan/VulkanObject.h"

#include "Managers/InputManager.h"

// Standard library includes
#include <chrono>
#include <thread>

DDM::DDMEngine::DDMEngine()
{
}

DDM::DDMEngine::~DDMEngine()
{
	VulkanObject::GetInstance().Terminate();

}

void DDM::DDMEngine::Run(const std::function<void()>& load)
{
	// If not initialized, return
	if (!m_Initialized)
	{
		std::cout << "Engine not initialized \n";
		return;
	}

	// Call load function
	load();

	// Get necesarry objects for later use
	auto& vulkanObject{ VulkanObject::GetInstance() };
	auto& sceneManager{ SceneManager::GetInstance() };
	auto& time{ TimeManager::GetInstance() };
	auto& window{ Window::GetInstance() };
	auto& input{ InputManager::GetInstance() };

	// Indicates wether the update loop should continue
	bool doContinue = true;

	// The timestamp of the previous frame
	auto lastTime = std::chrono::high_resolution_clock::now();

	// Time lag for physics update
	float lag = 0.0f;

	// Fixed step for the physics update
	constexpr float fixedTimeStep = 0.02f;

	// Set the fixed time
	time.SetFixedTime(fixedTimeStep);

	// Indicates wether desired framerate should be followed
	constexpr bool followDesiredFramerate = false;

	// Desired framerate
	constexpr float desiredFrameRate = 144.f;

	// Desired duration of each frame
	constexpr float desiredFrameDuration = 1000.f / desiredFrameRate;


	while (doContinue)
	{
		// Get timestamp of current frame and calculate delta time
		const auto currentTime = std::chrono::high_resolution_clock::now();
		const float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

		// Set the deltatime
		time.SetDeltaTime(deltaTime);

		// Process the input
		input.ProcessInput();

		// Set last time to current time for next frame
		lastTime = currentTime;

		// Add deltatime to lag
		lag += deltaTime;

		// Check if window should close
		doContinue = !glfwWindowShouldClose(window.GetWindowStruct().pWindow);

		// Poll glfw events
		glfwPollEvents();

		// Start the frame, used to initialize objects
		sceneManager.StartFrame();

		// Call early update
		sceneManager.EarlyUpdate();

		// Handle the fixed update
		while (lag >= fixedTimeStep)
		{
			sceneManager.FixedUpdate();
			lag -= fixedTimeStep;
		}

		// Call update
		sceneManager.Update();

		// Call late update
		sceneManager.LateUpdate();

		// Call post update, used
		sceneManager.PostUpdate();

		// Render scene
		vulkanObject.Render();

		// Calculate the duration of the frame so far
		const auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - currentTime);
		
		// Calculate how long the thread should sleep to keep desired frame duration
		const auto sleepTime = std::chrono::milliseconds(static_cast<int>(desiredFrameDuration)) - frameDuration;

		// Only sleep if desired framerate should be followed
		// and sleeptime  is over 0
		if (followDesiredFramerate && sleepTime > std::chrono::milliseconds(0))
		{
			// Sleep for the calculated amount of time
			std::this_thread::sleep_for(sleepTime);
		}
	}

	// Clean up all objects
	sceneManager.EndProgram();
}
