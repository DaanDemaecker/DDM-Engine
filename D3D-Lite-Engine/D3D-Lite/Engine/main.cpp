// File includes
#include "D3DEngine.h"
#include "SceneLoaders/LoadModelLoaderScene.h"
#include "SceneLoaders/LoadTestScene.h"

int main()
{
	// Create the engine object and run it with the load function
	DDM3::D3DEngine engine{};
	engine.Run(LoadTestScene::loadTestScene);
	//engine.Run(LoadModelLoaderScene::LoadModelLoaderScene);
	
	return EXIT_SUCCESS;
} 