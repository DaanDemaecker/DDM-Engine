#include "Vulkan/VulkanObject.h"
#include "../../Managers/SceneManager.h"

#include "../../DataTypes/Materials/Material.h"
#include "../../DataTypes/Materials/TexturedMaterial.h"
#include "../../DataTypes/Materials/MultiMaterial.h"

#include "../../Components/MeshRenderComponent.h"
#include "../../DataTypes/Mesh.h"
#include "../../Components/TransformComponent.h"
#include "../Scene.h"
#include "../../Components/SkyBoxComponent.h"

#include "../../Components/CameraComponent.h"
#include "../../Components/DirectionalLightComponent.h"

#include "../../Components/RotatorComponent.h"
#include "../../Components/SpectatorMovementComponent.h"

#include "../../Managers/ResourceManager.h"
#include "../../Managers/ConfigManager.h"
#include "Engine/DDMModelLoader.h"

#include "Components/MaterialSwitcher/MaterialSwitchManager.h"
#include "Components/MaterialSwitcher/MaterialSwitcher.h"
#include "Components/InfoComponent.h"

#include "Vulkan/Renderers/AORenderers/SSAORenderer.h"

namespace LoadSSAOScene
{
	void SetupPipelines();

	void SetupAtrium(DDM3::Scene* scene);

	void SetupInfoComponent(DDM3::Scene* scene);

	void SetupCamera(DDM3::Scene* scen);

	void SetupLight(DDM3::Scene* scene);

	void LoadScene()
	{
		auto scene = DDM3::SceneManager::GetInstance().CreateScene("Test");
		DDM3::SceneManager::GetInstance().SetActiveScene(scene);

		SetupPipelines();

		SetupAtrium(scene.get());

		SetupInfoComponent(scene.get());

		SetupCamera(scene.get());

		SetupLight(scene.get());
	}

	void SetupPipelines()
	{
		auto& vulkanObject{ DDM3::VulkanObject::GetInstance() };

		vulkanObject.AddGraphicsPipeline("DeferredDiffuse", { "Resources/Shaders/SSAOGbuffer.Vert.spv", "Resources/Shaders/SSAODiffuse.frag.spv" }, true, false, DDM3::SSAORenderer::kSubpass_GBUFFER);
	}

	void SetupAtrium(DDM3::Scene* scene)
	{
		auto& modelLoader = DDM3::DDMModelLoader::GetInstance();

		std::vector<std::unique_ptr<DDMML::Mesh>> pMeshes{};

		modelLoader.LoadScene("Resources/Models/SponzaAtrium/Sponza.gltf", pMeshes);

		auto sceneRoot = scene->GetSceneRoot();

		for (auto& pMesh : pMeshes)
		{
			auto pGameObject = scene->GetSceneRoot()->CreateNewObject(pMesh->GetName());
			auto renderComponent = pGameObject->AddComponent<DDM3::MeshRenderComponent>();
			renderComponent->SetMesh(pMesh.get());


			auto texturedMaterial = std::make_shared<DDM3::TexturedMaterial>("Default");

			for (auto& texture : pMesh->GetDiffuseTextureNames())
			{
				texturedMaterial->AddTexture(texture);
			}

			renderComponent->SetMaterial(texturedMaterial);
		}
	}


	void SetupInfoComponent(DDM3::Scene* scene)
	{
		auto pInfoObject{ scene->CreateGameObject("InfoComponent") };
		pInfoObject->SetShowImGui(true);

		auto pInfoComponent{ pInfoObject->AddComponent<DDM3::InfoComponent>() };
		pInfoComponent->SetShowImGui(true);
	}

	void SetupCamera(DDM3::Scene* scene)
	{
		auto pCamera{ scene->CreateGameObject("Camera") };

		pCamera->AddComponent<DDM3::SpectatorMovementComponent>();

		auto pCameraComponent{ pCamera->AddComponent<DDM3::CameraComponent>() };

		auto pCameraTransform{ pCamera->GetTransform() };
		pCameraTransform->SetLocalPosition(0, 5, -5);
		//pCameraTransform->SetLocalRotation(glm::vec3(0.0f, glm::radians(180.f), 0.0f));

		//pCamera->AddComponent<D3D::RotatorComponent>();

		scene->SetCamera(pCameraComponent);
		
		//auto& configManager{ DDM3::ConfigManager::GetInstance() };
		//
		//// Set the vertex shader name
		//const std::string vertShaderName{ configManager.GetString("SkyboxVertName") };
		//// Set the fragment shader name
		//const std::string fragShaderName{ configManager.GetString("SkyboxFragName") };
		//
		//// Create the graphics pipeline for the skybox
		//DDM3::VulkanObject::GetInstance().AddGraphicsPipeline(configManager.GetString("SkyboxPipelineName"), { vertShaderName, fragShaderName }, false);
		//
		//auto pSkyBox{ pCamera->AddComponent<DDM3::SkyBoxComponent>() };
		//
		//pSkyBox->LoadSkybox(std::initializer_list<const std::string>{"resources/images/CubeMap/Sky_Right.png",
		//	"resources/images/CubeMap/Sky_Left.png",
		//	"resources/images/CubeMap/Sky_Up.png",
		//	"resources/images/CubeMap/Sky_Down.png",
		//	"resources/images/CubeMap/Sky_Front.png",
		//	"resources/images/CubeMap/Sky_Back.png"});
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
		pLightTransform->SetLocalRotation(glm::vec3(glm::radians(45.f), glm::radians(45.f), 0.0f));


		//pLight->AddComponent<D3D::RotatorComponent>();

		scene->SetLight(pLightComponent);
	}
}