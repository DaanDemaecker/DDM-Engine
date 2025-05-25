// D3DEngine.cpp

// Header include
#include "DDM3Engine.h"

// File includes
#include "Scene.h"
#include "Window.h"


#include "Managers/SceneManager.h"
#include "Managers/ConfigManager.h"
#include "Managers/TimeManager.h"


#include "Vulkan/VulkanObject.h"

#include "Managers/InputManager.h"

#include "Vulkan/Renderers/DefaultRenderer.h"

// Standard library includes
#include <chrono>
#include <thread>

DDM3::DDM3Engine::DDM3Engine()
{
}

DDM3::DDM3Engine::~DDM3Engine()
{
	VulkanObject::GetInstance().Terminate();

}

void DDM3::DDM3Engine::Run(const std::function<void()>& load)
{
	if (!m_Initialized)
	{
		std::cout << "Engine not initialized \n";
		return;
	}

	load();

	auto& renderer{ VulkanObject::GetInstance() };
	auto& sceneManager{ SceneManager::GetInstance() };
	auto& time{ TimeManager::GetInstance() };
	auto& window{ Window::GetInstance() };
	auto& input{ InputManager::GetInstance() };


	bool doContinue = true;
	auto lastTime = std::chrono::high_resolution_clock::now();

	float lag = 0.0f;
	constexpr float fixedTimeStep = 0.02f;

	time.SetFixedTime(fixedTimeStep);

	constexpr float desiredFrameRate = 144.f;
	constexpr float desiredFrameDuration = 1000.f / desiredFrameRate;


	while (doContinue)
	{
		const auto currentTime = std::chrono::high_resolution_clock::now();
		const float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

		time.SetDeltaTime(deltaTime);

		input.ProcessInput();

		//std::cout << time.GetFps() << std::endl;

		lastTime = currentTime;
		lag += deltaTime;

		doContinue = !glfwWindowShouldClose(window.GetWindowStruct().pWindow);

		glfwPollEvents();

		sceneManager.StartFrame();

		sceneManager.EarlyUpdate();

		while (lag >= fixedTimeStep)
		{
			sceneManager.FixedUpdate();
			lag -= fixedTimeStep;
		}

		sceneManager.Update();

		sceneManager.LateUpdate();

		sceneManager.PostUpdate();

		sceneManager.Cleanup();

		sceneManager.PrepareRender();

		renderer.Render();

		if (input.GetKeyUp(GLFW_KEY_SPACE))
		{
			std::cout << "yes" << std::endl;
		}

		const auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - currentTime);
		const auto sleepTime = std::chrono::milliseconds(static_cast<int>(desiredFrameDuration)) - frameDuration;

		if (sleepTime > std::chrono::milliseconds(0))
		{
			std::this_thread::sleep_for(sleepTime);
		}
	}

	sceneManager.EndProgram();
}
