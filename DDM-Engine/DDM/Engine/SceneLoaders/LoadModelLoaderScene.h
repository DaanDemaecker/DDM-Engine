#include "Vulkan/VulkanObject.h"
#include "Managers/SceneManager.h"
#include "../Scene.h"
#include "Components/SpectatorMovement.h"
#include "Components/Camera.h"
#include "Components/SkyBox.h"
#include "Components/Transform.h"
#include "Components/Light/LightComponent.h"
#include "Components/ModelLoader.h"
#include "../../Managers/ConfigManager.h"

namespace LoadModelLoaderScene
{
	void SetupPipelines();

	void SetupCamera(DDM::Scene* scene);

	void SetupLight(DDM::Scene* scene);

	void SetupModelLoader(DDM::Scene* scene);

	void LoadModelLoaderScene()
	{
		auto scene = DDM::SceneManager::GetInstance().CreateScene("ModelLoader");
		DDM::SceneManager::GetInstance().SetActiveScene(scene);

		SetupPipelines();
		
		SetupCamera(scene.get());

		SetupLight(scene.get());

		SetupModelLoader(scene.get());
	}

	void SetupPipelines()
	{
		auto& renderer{ DDM::VulkanObject::GetInstance() };
		
		renderer.AddGraphicsPipeline("Diffuse", { "Resources/Shaders/Diffuse.Vert.spv", "Resources/Shaders/Diffuse.Frag.spv" });
		renderer.AddGraphicsPipeline("NormalMap", { "Resources/Shaders/NormalMap.Vert.spv", "Resources/Shaders/NormalMap.Frag.spv" });
		renderer.AddGraphicsPipeline("DiffNorm", { "Resources/Shaders/DiffNorm.Vert.spv", "Resources/Shaders/DiffNorm.Frag.spv" });
		renderer.AddGraphicsPipeline("Test", { "Resources/Shaders/Test.Vert.spv", "Resources/Shaders/Test.Frag.spv" });
		renderer.AddGraphicsPipeline("DiffuseUnshaded", { "Resources/Shaders/DiffuseUnshaded.Vert.spv", "Resources/Shaders/DiffuseUnshaded.Frag.spv" });
		renderer.AddGraphicsPipeline("Specular", { "Resources/Shaders/Specular.Vert.spv", "Resources/Shaders/Specular.Frag.spv" });
		renderer.AddGraphicsPipeline("DiffNormSpec", { "Resources/Shaders/DiffNormSpec.Vert.spv", "Resources/Shaders/DiffNormSpec.Frag.spv" });
		renderer.AddGraphicsPipeline("MultiShader", { "Resources/Shaders/MultiShader.Vert.spv", "Resources/Shaders/MultiShader.Frag.spv" });
	}

	void SetupCamera(DDM::Scene* scene)
	{
		auto pCamera{ scene->CreateGameObject("Camera") };

		pCamera->AddComponent<DDM::SpectatorMovement>();

		auto pCameraComponent{ pCamera->AddComponent<DDM::Camera>() };

		auto pCameraTransform{ pCamera->GetTransform() };
		//pCameraTransform->SetLocalPosition(0, 1, 0);

		//pCamera->AddComponent<D3D::RotatorComponent>();

		scene->SetCamera(pCameraComponent);

		auto& configManager{ DDM::ConfigManager::GetInstance() };

		// Set the vertex shader name
		const std::string vertShaderName{ configManager.GetString("SkyboxVertName") };
		// Set the fragment shader name
		const std::string fragShaderName{ configManager.GetString("SkyboxFragName") };

		// Create the graphics pipeline for the skybox
		DDM::VulkanObject::GetInstance().AddGraphicsPipeline(configManager.GetString("SkyboxPipelineName"), { vertShaderName, fragShaderName }, false);

		auto pSkyBox{ pCamera->AddComponent<DDM::SkyBoxComponent>() };

		pSkyBox->SetRight("resources/images/Skybox/Sky_Right.png");
		pSkyBox->SetLeft("resources/images/Skybox/Sky_Left.png");
		pSkyBox->SetUp("resources/images/Skybox/Sky_Up.png");
		pSkyBox->SetDown("resources/images/Skybox/Sky_Down.png");
		pSkyBox->SetFront("resources/images/Skybox/Sky_Front.png");
		pSkyBox->SetBack("resources/images/Skybox/Sky_Back.png");
	}

	void SetupLight(DDM::Scene* scene)
	{
		auto pLight{ scene->CreateGameObject("Light") };

		pLight->SetShowImGui(true);

		auto pLightComponent{ pLight->AddComponent<DDM::LightComponent>() };
		pLightComponent->SetShowImGui(true);

		//pLightComponent->SetColor(glm::vec3{ 0, 0, 0 });

		auto pLightTransform{ pLight->GetTransform() };
		pLightTransform->SetShowImGui(true);

		pLightTransform->SetLocalRotation(glm::vec3(0.0f, glm::radians(45.f), 0.0f));


		//pLight->AddComponent<D3D::RotatorComponent>();

		scene->SetLight(pLightComponent);
	}

	void SetupModelLoader(DDM::Scene* scene)
	{
		auto pModelLoader{ scene->CreateGameObject("ModelLoader") };
		pModelLoader->SetShowImGui(true);

		auto pModelLoaderComponent{pModelLoader->AddComponent<DDM::ModelLoader>()};
		pModelLoaderComponent->SetShowImGui(true);
	}
}

