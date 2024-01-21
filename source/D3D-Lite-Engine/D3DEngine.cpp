#include "D3DEngine.h"
#include "VulkanRenderer.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "Material.h"
#include <chrono>

#include "Scene.h"

D3D::Window g_pWindow{};

D3D::D3DEngine::D3DEngine(int width, int height)
{
	g_pWindow.Width = width;
	g_pWindow.Height = height;
	InitWindow();
}

D3D::D3DEngine::~D3DEngine()
{
	glfwDestroyWindow(g_pWindow.pWindow);
	glfwTerminate();
}

void D3D::D3DEngine::Run(const std::function<void()>& load)
{
	load();

	auto& renderer{ VulkanRenderer::GetInstance() };
	auto& sceneManager{ SceneManager::GetInstance() };
	auto& time{ TimeManager::GetInstance() };


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

		//std::cout << time.GetFps() << std::endl;

		lastTime = currentTime;
		lag += deltaTime;

		doContinue = !glfwWindowShouldClose(g_pWindow.pWindow);

		glfwPollEvents();

		sceneManager.StartFrame();

		while (lag >= fixedTimeStep)
		{
			sceneManager.FixedUpdate();
			lag -= fixedTimeStep;
		}

		sceneManager.Update();

		sceneManager.LateUpdate();

		sceneManager.Cleanup();

		renderer.Render();

		sceneManager.Cleanup();

		const auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - currentTime);
		const auto sleepTime = std::chrono::milliseconds(static_cast<int>(desiredFrameDuration)) - frameDuration;

		if (sleepTime > std::chrono::milliseconds(0))
		{
			std::this_thread::sleep_for(sleepTime);
		}
	}

	sceneManager.EndProgram();
}

void D3D::D3DEngine::InitWindow()
{
	//Initialize glfw
	glfwInit();

	//Tell GLFW not to create an OpenGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Initialize the window
	g_pWindow.pWindow = glfwCreateWindow(g_pWindow.Width, g_pWindow.Height, "D3D Lite Engine", nullptr, nullptr);
	glfwSetWindowUserPointer(g_pWindow.pWindow, this);
	glfwSetFramebufferSizeCallback(g_pWindow.pWindow, FramebufferResizeCallback);
}

void D3D::D3DEngine::FramebufferResizeCallback(GLFWwindow* /*pWindow*/, int /*width*/, int /*height*/)
{
	g_pWindow.FrameBufferResized = true;
}
