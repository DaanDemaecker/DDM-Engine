#include "../Vulkan/VulkanRenderer.h"
#include "../Managers/SceneManager.h"
#include "../Scene.h"
#include "../../Components/SpectatorMovementComponent.h"
#include "../../Components/CameraComponent.h"
#include "../../Components/SkyBoxComponent.h"
#include "../../Components/TransformComponent.h"
#include "../../Components/DirectionalLightComponent.h"
#include "../../Components/ModelLoaderComponent.h"

namespace LoadModelLoaderScene
{
	void SetupPipelines();

	void SetupCamera(D3D::Scene* scene);

	void SetupLight(D3D::Scene* scene);

	void SetupModelLoader(D3D::Scene* scene);

	void LoadModelLoaderScene()
	{
		auto scene = D3D::SceneManager::GetInstance().CreateScene("ModelLoader");
		D3D::SceneManager::GetInstance().SetActiveScene(scene);

		SetupPipelines();
		
		SetupCamera(scene.get());

		SetupLight(scene.get());

		SetupModelLoader(scene.get());
	}

	void SetupPipelines()
	{
		auto& renderer{ D3D::VulkanRenderer::GetInstance() };
		
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

	void SetupCamera(D3D::Scene* scene)
	{
		auto pCamera{ scene->CreateGameObject("Camera") };

		pCamera->AddComponent<D3D::SpectatorMovementComponent>();

		auto pCameraComponent{ pCamera->AddComponent<D3D::CameraComponent>() };

		auto pCameraTransform{ pCamera->GetTransform() };
		//pCameraTransform->SetLocalRotation(glm::vec3(0.0f, glm::radians(180.f), 0.0f));

		//pCamera->AddComponent<D3D::RotatorComponent>();

		scene->SetCamera(pCameraComponent);

		// Set the vertex shader name
		const std::string vertShaderName{ "Resources/Shaders/Skybox.Vert.spv" };
		// Set the fragment shader name
		const std::string fragShaderName{ "Resources/Shaders/Skybox.Frag.spv" };

		// Create the graphics pipeline for the skybox
		D3D::VulkanRenderer::GetInstance().AddGraphicsPipeline("Skybox", { vertShaderName, fragShaderName }, false);

		auto pSkyBox{ pCamera->AddComponent<D3D::SkyBoxComponent>() };

		pSkyBox->LoadSkybox(std::initializer_list<const std::string>{"resources/images/CubeMap/Sky_Right.png",
			"resources/images/CubeMap/Sky_Left.png",
			"resources/images/CubeMap/Sky_Up.png",
			"resources/images/CubeMap/Sky_Down.png",
			"resources/images/CubeMap/Sky_Front.png",
			"resources/images/CubeMap/Sky_Back.png"});
	}

	void SetupLight(D3D::Scene* scene)
	{
		auto pLight{ scene->CreateGameObject("Light") };

		pLight->SetShowImGui(true);

		auto pLightComponent{ pLight->AddComponent<D3D::DirectionalLightComponent>() };
		pLightComponent->SetShowImGui(true);

		//pLightComponent->SetColor(glm::vec3{ 0, 0, 0 });

		auto pLightTransform{ pLight->GetTransform() };
		pLightTransform->SetShowImGui(true);

		pLightTransform->SetLocalRotation(glm::vec3(0.0f, glm::radians(45.f), 0.0f));


		//pLight->AddComponent<D3D::RotatorComponent>();

		scene->SetLight(pLightComponent);
	}

	void SetupModelLoader(D3D::Scene* scene)
	{
		auto pModelLoader{ scene->CreateGameObject("ModelLoader") };
		pModelLoader->SetShowImGui(true);

		auto pModelLoaderComponent{pModelLoader->AddComponent<D3D::ModelLoaderComponent>()};
		pModelLoaderComponent->SetShowImGui(true);
	}
}

