#include "Vulkan/VulkanObject.h"
#include "../Managers/SceneManager.h"
#include "../Scene.h"
#include "../../Components/SpectatorMovementComponent.h"
#include "../../Components/CameraComponent.h"
#include "../../Components/SkyBoxComponent.h"
#include "../../Components/TransformComponent.h"
#include "../../Components/DirectionalLightComponent.h"
#include "../../Components/ModelLoaderComponent.h"
#include "../../Managers/ConfigManager.h"

namespace LoadModelLoaderScene
{
	void SetupPipelines();

	void SetupCamera(DDM3::Scene* scene);

	void SetupLight(DDM3::Scene* scene);

	void SetupModelLoader(DDM3::Scene* scene);

	void LoadModelLoaderScene()
	{
		auto scene = DDM3::SceneManager::GetInstance().CreateScene("ModelLoader");
		DDM3::SceneManager::GetInstance().SetActiveScene(scene);

		SetupPipelines();
		
		SetupCamera(scene.get());

		SetupLight(scene.get());

		SetupModelLoader(scene.get());
	}

	void SetupPipelines()
	{
		auto& renderer{ DDM3::VulkanObject::GetInstance() };
		
		renderer.AddGraphicsPipeline("Diffuse", { "Resources/Shaders/Diffuse.Vert.spv", "Resources/Shaders/Diffuse.Frag.spv" });
		renderer.AddGraphicsPipeline("NormalMap", { "Resources/Shaders/NormalMap.Vert.spv", "Resources/Shaders/NormalMap.Frag.spv" });
		renderer.AddGraphicsPipeline("DiffNorm", { "Resources/Shaders/DiffNorm.Vert.spv", "Resources/Shaders/DiffNorm.Frag.spv" });
		renderer.AddGraphicsPipeline("Test", { "Resources/Shaders/Test.Vert.spv", "Resources/Shaders/Test.Frag.spv" });
		renderer.AddGraphicsPipeline("DiffuseUnshaded", { "Resources/Shaders/DiffuseUnshaded.Vert.spv", "Resources/Shaders/DiffuseUnshaded.Frag.spv" });
		renderer.AddGraphicsPipeline("Specular", { "Resources/Shaders/Specular.Vert.spv", "Resources/Shaders/Specular.Frag.spv" });
		renderer.AddGraphicsPipeline("DiffNormSpec", { "Resources/Shaders/DiffNormSpec.Vert.spv", "Resources/Shaders/DiffNormSpec.Frag.spv" });
		renderer.AddGraphicsPipeline("MultiShader", { "Resources/Shaders/MultiShader.Vert.spv", "Resources/Shaders/MultiShader.Frag.spv" });
		renderer.AddGraphicsPipeline("Skinned", { "Resources/Shaders/Skinned.Vert.spv", "Resources/Shaders/MultiShader.Frag.spv" });

	}

	void SetupCamera(DDM3::Scene* scene)
	{
		auto pCamera{ scene->CreateGameObject("Camera") };

		pCamera->AddComponent<DDM3::SpectatorMovementComponent>();

		auto pCameraComponent{ pCamera->AddComponent<DDM3::CameraComponent>() };

		auto pCameraTransform{ pCamera->GetTransform() };
		//pCameraTransform->SetLocalRotation(glm::vec3(0.0f, glm::radians(180.f), 0.0f));

		//pCamera->AddComponent<D3D::RotatorComponent>();

		scene->SetCamera(pCameraComponent);

		auto& configManager{ DDM3::ConfigManager::GetInstance() };

		// Set the vertex shader name
		const std::string vertShaderName{ configManager.GetString("SkyboxVertName") };
		// Set the fragment shader name
		const std::string fragShaderName{ configManager.GetString("SkyboxFragName") };

		// Create the graphics pipeline for the skybox
		DDM3::VulkanObject::GetInstance().AddGraphicsPipeline(configManager.GetString("SkyboxPipelineName"), { vertShaderName, fragShaderName }, false);

		auto pSkyBox{ pCamera->AddComponent<DDM3::SkyBoxComponent>() };

		pSkyBox->LoadSkybox(std::initializer_list<const std::string>{"resources/images/CubeMap/Sky_Right.png",
			"resources/images/CubeMap/Sky_Left.png",
			"resources/images/CubeMap/Sky_Up.png",
			"resources/images/CubeMap/Sky_Down.png",
			"resources/images/CubeMap/Sky_Front.png",
			"resources/images/CubeMap/Sky_Back.png"});
	}

	void SetupLight(DDM3::Scene* scene)
	{
		auto pLight{ scene->CreateGameObject("Light") };

		pLight->SetShowImGui(true);

		auto pLightComponent{ pLight->AddComponent<DDM3::DirectionalLightComponent>() };
		pLightComponent->SetShowImGui(true);

		//pLightComponent->SetColor(glm::vec3{ 0, 0, 0 });

		auto pLightTransform{ pLight->GetTransform() };
		pLightTransform->SetShowImGui(true);

		pLightTransform->SetLocalRotation(glm::vec3(0.0f, glm::radians(45.f), 0.0f));


		//pLight->AddComponent<D3D::RotatorComponent>();

		scene->SetLight(pLightComponent);
	}

	void SetupModelLoader(DDM3::Scene* scene)
	{
		auto pModelLoader{ scene->CreateGameObject("ModelLoader") };
		pModelLoader->SetShowImGui(true);

		auto pModelLoaderComponent{pModelLoader->AddComponent<DDM3::ModelLoaderComponent>()};
		pModelLoaderComponent->SetShowImGui(true);
	}
}

