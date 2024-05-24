#include "D3DEngine.h"
#include "SceneLoaders/LoadTestScene.h"

int main()
{
	// Create the engine object and run it with the load function
	D3D::D3DEngine engine{};
	engine.Run(loadTestScene);

	return EXIT_SUCCESS;
} 