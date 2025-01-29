// D3DEngine.cpp

// Header include
#include "D3DEngine.h"

// File includes
#include "Scene.h"
#include "Window.h"

#include "ServiceLocator/ModelLoader/D3DModelLoader.h"
#include "ServiceLocator/ServiceLocator.h"

#include "Managers/SceneManager.h"
#include "Managers/ConfigManager.h"
#include "Managers/TimeManager.h"

#include "Vulkan/VulkanRenderer.h"

#include "Managers/InputManager.h"

// Standard library includes
#include <chrono>
#include <thread>

DDM3::D3DEngine::D3DEngine()
{
	// Create the window with the given width and height
	DDM3::Window::GetInstance();

	ServiceLocator::RegisterModelLoader(std::make_unique<D3DModelLoader>());

	auto& renderer{ DDM3::VulkanRenderer::GetInstance() };

	renderer.AddDefaultPipeline();
}

DDM3::D3DEngine::~D3DEngine()
{

}

void DDM3::D3DEngine::Run(const std::function<void()>& load)
{
	load();

	auto& renderer{ VulkanRenderer::GetInstance() };
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
