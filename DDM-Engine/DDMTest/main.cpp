// Main.cpp

// File includes
#include "DDM-Engine/Engine/DDMEngine.h"

#include "SceneLoaders/LoadModelLoaderScene.h"
#include "SceneLoaders/LoadTestScene.h"
#include "SceneLoaders/LoadDeferredScene.h"
#include "SceneLoaders/LoadAOScene.h"

#include "DDM-Engine/Vulkan/Renderers/ForwardRenderer.h"
#include "DDM-Engine/Vulkan/Renderers/DeferredRenderer.h"
#include "DDM-Engine/Vulkan/Renderers/AORenderers/SSAORenderer.h"
#include "DDM-Engine/Vulkan/Renderers/AORenderers/HBAORenderer.h"
#include "DDM-Engine/Vulkan/Renderers/AORenderers/GTAORenderer.h"

#include "DDM-Engine/ServiceLocator/ServiceLocator.h"
#include "DDM-Engine/ServiceLocator/SoundSystem/Fmod/FmodSoundSystem.h"

enum
{
	activeRendererForward = 0,
	activeRendererDeffered = 1,
	activeRendererSSAO = 2,
	activeRendererHBAO = 3,
	activeRendererGTAO = 4
};

int main()
{
	// Seed random number generator
	srand(static_cast<uint16_t>(time(NULL)));

	DDM::ServiceLocator::RegisterSoundSystem(std::make_unique<DDM::FmodSoundSystem>());

	// Create the engine object and run it with the load function
	DDM::DDMEngine engine{};

	int activeRenderer{ activeRendererSSAO };

	switch (activeRenderer)
	{
	case activeRendererForward:
		engine.Init<DDM::ForwardRenderer>();
		engine.Run(LoadTestScene::loadTestScene);
		//engine.Run(LoadModelLoaderScene::LoadModelLoaderScene);
		break;
	case activeRendererDeffered:
		engine.Init<DDM::DeferredRenderer>();
		engine.Run(LoadDeferredScene::LoadScene);
		break;
	case activeRendererSSAO:
		engine.Init<DDM::SSAORenderer>();
		engine.Run(LoadAOScene::LoadScene);
		break;
	case activeRendererHBAO:
		engine.Init<DDM::HBAORenderer>();
		engine.Run(LoadAOScene::LoadScene);
		break;
	case activeRendererGTAO:
		engine.Init<DDM::GTAORenderer>();
		engine.Run(LoadAOScene::LoadScene);
		break;
	default:
		break;
	}

	DDM::ResourceManager::GetInstance().EndProgram();

	return EXIT_SUCCESS;
} 