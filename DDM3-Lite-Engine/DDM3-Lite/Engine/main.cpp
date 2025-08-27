// File includes
#include "DDM3Engine.h"

#include "SceneLoaders/LoadModelLoaderScene.h"
#include "SceneLoaders/LoadTestScene.h"
#include "SceneLoaders/LoadDeferredScene.h"
#include "SceneLoaders/LoadAOScene.h"

#include "Vulkan/Renderers/ForwardRenderer.h"
#include "Vulkan/Renderers/DeferredRenderer.h"
#include "Vulkan/Renderers/AORenderers/SSAORenderer.h"
#include "Vulkan/Renderers/AORenderers/HBAORenderer.h"
#include "Vulkan/Renderers/AORenderers/GTAORenderer.h"

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
	srand(time(NULL));

	// Create the engine object and run it with the load function
	DDM::DDM3Engine engine{};

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

	
	
	return EXIT_SUCCESS;
} 