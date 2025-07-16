// File includes
#include "DDM3Engine.h"
#include "SceneLoaders/LoadModelLoaderScene.h"
#include "SceneLoaders/LoadTestScene.h"
#include "SceneLoaders/LoadDeferredScene.h"
#include "SceneLoaders/LoadSSAOScene.h"

#include "Vulkan/Renderers/ForwardRenderer.h"
#include "Vulkan/Renderers/DeferredRenderer.h"
#include "Vulkan/Renderers/SSAORenderer.h"

int main()
{
	// Create the engine object and run it with the load function
	DDM3::DDM3Engine engine{};

	//engine.Init<DDM3::DeferredRenderer>();
	//engine.Run(LoadDeferredScene::LoadScene);

	engine.Init<DDM3::SSAORenderer>();
	engine.Run(LoadSSAOScene::LoadScene);
	
	//engine.Init<DDM3::ForwardRenderer>();
	//engine.Run(LoadTestScene::loadTestScene);
	//engine.Run(LoadModelLoaderScene::LoadModelLoaderScene);
	
	return EXIT_SUCCESS;
} 