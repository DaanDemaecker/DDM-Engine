#include "stdafx.h"

#include "D3DEngine.h"
#include "VulkanRenderer.h"
#include "SceneManager.h"

void load()
{
	auto scene = D3D::SceneManager::GetInstance().CreateScene("Test");
	D3D::SceneManager::GetInstance().SetActiveScene(scene);
}

int main()
{
	const uint32_t windowWidth = 800;
	const uint32_t windowHeight = 600;

	D3D::D3DEngine engine(static_cast<int>(windowWidth), static_cast<int>(windowHeight));
	engine.Run(load);

	return EXIT_SUCCESS;
} 