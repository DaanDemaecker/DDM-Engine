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

#include "Vulkan/Renderers/AORenderers/AoRenderPasses.h"

namespace LoadAOScene
{
	void SetupPipelines();

	void SetupAtrium(DDM3::Scene* scene);

	void SetupRoom(DDM3::Scene* scene);

	void SetupVehicle(DDM3::Scene* scene);

	void SetupInfoComponent(DDM3::Scene* scene);

	void SetupCamera(DDM3::Scene* scen);

	void SetupLight(DDM3::Scene* scene);

	void LoadScene()
	{
		auto scene = DDM3::SceneManager::GetInstance().CreateScene("Test");
		DDM3::SceneManager::GetInstance().SetActiveScene(scene);

		SetupPipelines();

		SetupAtrium(scene.get());

		//SetupRoom(scene.get());

		//SetupVehicle(scene.get());

		SetupInfoComponent(scene.get());

		SetupCamera(scene.get());

		SetupLight(scene.get());
	}

	void SetupPipelines()
	{
		auto& vulkanObject{ DDM3::VulkanObject::GetInstance() };

		vulkanObject.AddGraphicsPipeline("DeferredDiffuse", { "Resources/Shaders/AO/AOGbuffer.Vert.spv", "Resources/Shaders/AO/AODiffuse.frag.spv" }, true, false, DDM3::kSubpass_GBUFFER);
	}

	void SetupAtrium(DDM3::Scene* scene)
	{
		auto switchManager = scene->GetSceneRoot()->CreateNewObject("Material switch manager");
		switchManager->SetShowImGui(true);

		auto switchManagerComponent = switchManager->AddComponent<DDM3::MaterialSwitchManager>();
		switchManagerComponent->RegisterKey("Diffuse");
		switchManagerComponent->RegisterKey("Default");


		auto& modelLoader = DDM3::DDMModelLoader::GetInstance();

		std::vector<std::unique_ptr<DDMML::Mesh>> pMeshes{};

		modelLoader.LoadScene("Resources/Models/SponzaAtrium/Sponza.gltf", pMeshes);

		auto sceneRoot = scene->GetSceneRoot();

		for (auto& pMesh : pMeshes)
		{
			auto pGameObject = scene->GetSceneRoot()->CreateNewObject(pMesh->GetName());
			auto renderComponent = pGameObject->AddComponent<DDM3::MeshRenderComponent>();
			renderComponent->SetMesh(pMesh.get());


			auto texturedMaterial = std::make_shared<DDM3::TexturedMaterial>("DeferredDiffuse");

			for (auto& texture : pMesh->GetDiffuseTextureNames())
			{
				texturedMaterial->AddTexture(texture);
			}

			auto defaultMaterial = std::make_shared<DDM3::Material>();

			renderComponent->SetMaterial(texturedMaterial);

			auto pMaterialSwitcher = pGameObject->AddComponent<DDM3::MaterialSwitcher>();
			pMaterialSwitcher->RegisterMaterial("Diffuse", texturedMaterial);
			pMaterialSwitcher->RegisterMaterial("Default", defaultMaterial);


			switchManagerComponent->RegisterMaterialSwitcher(pMaterialSwitcher);
		}
	}

	void SetupRoom(DDM3::Scene* scene)
	{
		auto switchManager = scene->GetSceneRoot()->CreateNewObject("Material switch manager");
		switchManager->SetShowImGui(true);

		auto switchManagerComponent = switchManager->AddComponent<DDM3::MaterialSwitchManager>();
		switchManagerComponent->RegisterKey("Diffuse");
		switchManagerComponent->RegisterKey("Default");


		auto& modelLoader = DDM3::DDMModelLoader::GetInstance();

		std::vector<std::unique_ptr<DDMML::Mesh>> pMeshes{};

		modelLoader.LoadScene("Resources/Models/Room/scene.gltf", pMeshes);

		auto sceneRoot = scene->GetSceneRoot();

		for (int i{}; i < 1; ++i)
		{
			auto& pMesh = pMeshes[i];

			auto pGameObject = scene->GetSceneRoot()->CreateNewObject(pMesh->GetName());
			auto renderComponent = pGameObject->AddComponent<DDM3::MeshRenderComponent>();
			renderComponent->SetMesh(pMesh.get());


			std::shared_ptr<DDM3::Material> texturedMaterial = std::make_shared<DDM3::TexturedMaterial>("DeferredDiffuse");


			if (pMesh->GetDiffuseTextureNames().size() > 0)
			{
				for (auto& texture : pMesh->GetDiffuseTextureNames())
				{
					dynamic_pointer_cast<DDM3::TexturedMaterial>(texturedMaterial)->AddTexture(texture);
				}
			}
			else
			{
				texturedMaterial = std::make_shared<DDM3::Material>();
			}


			renderComponent->SetMaterial(texturedMaterial);

			auto defaultMaterial = std::make_shared<DDM3::Material>();


			auto pMaterialSwitcher = pGameObject->AddComponent<DDM3::MaterialSwitcher>();
			pMaterialSwitcher->RegisterMaterial("Diffuse", texturedMaterial);
			pMaterialSwitcher->RegisterMaterial("Default", defaultMaterial);


			switchManagerComponent->RegisterMaterialSwitcher(pMaterialSwitcher);

			auto pRoomTransform{ pGameObject->GetTransform() };
			pRoomTransform->SetLocalScale(0.05f, 0.05f, 0.05f);
		}
	}

	void SetupVehicle(DDM3::Scene* scene)
	{
		auto texturedMaterial = std::make_shared<DDM3::TexturedMaterial>("DeferredDiffuse");
		
		texturedMaterial->AddTexture("resources/images/vehicle_diffuse.png");
		
		auto pVehicle{ DDM3::DDMModelLoader::GetInstance().LoadModel("Resources/Models/vehicle.obj", scene->GetSceneRoot()) };
		pVehicle->SetShowImGui(true);
		
		auto pVehicleModel{ pVehicle->GetComponent<DDM3::MeshRenderComponent>() };
		pVehicleModel->SetMaterial(texturedMaterial);
		
		
		auto pVehicleTransform{ pVehicle->GetTransform() };
		pVehicleTransform->SetShowImGui(true);
		pVehicleTransform->SetLocalPosition(3, 3, 0);
		pVehicleTransform->SetLocalRotation(0.f, glm::radians(75.0f), 0.f);
		pVehicleTransform->SetLocalScale(0.05f, 0.05f, 0.05f);
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
		pCameraTransform->SetLocalPosition(8, 1.5, -0.3);
		pCameraTransform->SetLocalRotation(0.0f, glm::radians(90.0f), 0.0f);

		//pCameraTransform->SetLocalPosition(3.82492f, 5.96203f, -3.80923);
		//pCameraTransform->SetLocalRotation(-2.73159, 0.0011386, 3.14159);

		//pCamera->AddComponent<D3D::RotatorComponent>();

		scene->SetCamera(pCameraComponent);
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