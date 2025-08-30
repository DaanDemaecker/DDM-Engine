#include "Vulkan/VulkanObject.h"
#include "../../Managers/SceneManager.h"

#include "../../DataTypes/Materials/Material.h"
#include "../../DataTypes/Materials/TexturedMaterial.h"
#include "../../DataTypes/Materials/MultiMaterial.h"

#include "Components/MeshRenderer.h"
#include "../../DataTypes/Mesh.h"
#include "../../Components/TransformComponent.h"
#include "../Scene.h"
#include "Components/SkyBox.h"

#include "Components/Camera.h"
#include "Components/Light/LightComponent.h"

#include "Components/Rotator.h"
#include "Components/SpectatorMovement.h"

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

	void SetupAtrium(DDM::Scene* scene);

	void SetupRoom(DDM::Scene* scene);

	void SetupVehicle(DDM::Scene* scene);

	void SetupInfoComponent(DDM::Scene* scene);

	void SetupCamera(DDM::Scene* scen);

	void SetupLight(DDM::Scene* scene);

	void SetupGroundPlane(DDM::Scene* scene);

	void SetupGear(DDM::Scene* scene);

	void LoadScene()
	{
		auto scene = DDM::SceneManager::GetInstance().CreateScene("Test");
		DDM::SceneManager::GetInstance().SetActiveScene(scene);

		SetupPipelines();

		SetupAtrium(scene.get());

		//SetupRoom(scene.get());

		//SetupVehicle(scene.get());

		//SetupGear(scene.get());

		//SetupGroundPlane(scene.get());

		SetupInfoComponent(scene.get());

		SetupCamera(scene.get());

		SetupLight(scene.get());
	}

	void SetupPipelines()
	{
		auto& vulkanObject{ DDM::VulkanObject::GetInstance() };

		vulkanObject.AddGraphicsPipeline("DeferredDiffuse", { "Resources/Shaders/AO/AOGbuffer.Vert.spv", "Resources/Shaders/AO/AODiffuse.frag.spv" }, true, false, DDM::kSubpass_GBUFFER);
	}

	void SetupAtrium(DDM::Scene* scene)
	{
		auto switchManager = scene->GetSceneRoot()->CreateNewObject("Material switch manager");
		switchManager->SetShowImGui(true);

		auto switchManagerComponent = switchManager->AddComponent<DDM::MaterialSwitchManager>();
		switchManagerComponent->RegisterKey("Diffuse");
		switchManagerComponent->RegisterKey("Default");


		auto& modelLoader = DDM::DDMModelLoader::GetInstance();

		std::vector<std::unique_ptr<DDMML::Mesh>> pMeshes{};

		modelLoader.LoadScene("Resources/Models/SponzaAtrium/Sponza.gltf", pMeshes);

		auto sceneRoot = scene->GetSceneRoot();

		for (auto& pMesh : pMeshes)
		{
			auto pGameObject = scene->GetSceneRoot()->CreateNewObject(pMesh->GetName());
			auto renderComponent = pGameObject->AddComponent<DDM::MeshRenderComponent>();
			renderComponent->SetMesh(pMesh.get());


			auto texturedMaterial = std::make_shared<DDM::TexturedMaterial>("DeferredDiffuse");

			for (auto& texture : pMesh->GetDiffuseTextureNames())
			{
				texturedMaterial->AddTexture(texture);
			}

			auto defaultMaterial = std::make_shared<DDM::Material>();

			renderComponent->SetMaterial(texturedMaterial);

			auto pMaterialSwitcher = pGameObject->AddComponent<DDM::MaterialSwitcher>();
			pMaterialSwitcher->RegisterMaterial("Diffuse", texturedMaterial);
			pMaterialSwitcher->RegisterMaterial("Default", defaultMaterial);


			switchManagerComponent->RegisterMaterialSwitcher(pMaterialSwitcher);
		}
	}

	void SetupRoom(DDM::Scene* scene)
	{
		auto switchManager = scene->GetSceneRoot()->CreateNewObject("Material switch manager");
		switchManager->SetShowImGui(true);

		auto switchManagerComponent = switchManager->AddComponent<DDM::MaterialSwitchManager>();
		switchManagerComponent->RegisterKey("Diffuse");
		switchManagerComponent->RegisterKey("Default");


		auto& modelLoader = DDM::DDMModelLoader::GetInstance();

		std::vector<std::unique_ptr<DDMML::Mesh>> pMeshes{};

		modelLoader.LoadScene("Resources/Models/Room/scene.gltf", pMeshes);

		auto sceneRoot = scene->GetSceneRoot();

		for (int i{}; i < 1; ++i)
		{
			auto& pMesh = pMeshes[i];

			auto pGameObject = scene->GetSceneRoot()->CreateNewObject(pMesh->GetName());
			auto renderComponent = pGameObject->AddComponent<DDM::MeshRenderComponent>();
			renderComponent->SetMesh(pMesh.get());


			std::shared_ptr<DDM::Material> texturedMaterial = std::make_shared<DDM::TexturedMaterial>("DeferredDiffuse");


			if (pMesh->GetDiffuseTextureNames().size() > 0)
			{
				for (auto& texture : pMesh->GetDiffuseTextureNames())
				{
					dynamic_pointer_cast<DDM::TexturedMaterial>(texturedMaterial)->AddTexture(texture);
				}
			}
			else
			{
				texturedMaterial = std::make_shared<DDM::Material>();
			}


			renderComponent->SetMaterial(texturedMaterial);

			auto defaultMaterial = std::make_shared<DDM::Material>();


			auto pMaterialSwitcher = pGameObject->AddComponent<DDM::MaterialSwitcher>();
			pMaterialSwitcher->RegisterMaterial("Diffuse", texturedMaterial);
			pMaterialSwitcher->RegisterMaterial("Default", defaultMaterial);


			switchManagerComponent->RegisterMaterialSwitcher(pMaterialSwitcher);

			auto pRoomTransform{ pGameObject->GetTransform() };
			pRoomTransform->SetLocalScale(0.05f, 0.05f, 0.05f);
		}
	}

	void SetupVehicle(DDM::Scene* scene)
	{
		auto switchManager = scene->GetSceneRoot()->CreateNewObject("Material switch manager");
		switchManager->SetShowImGui(true);

		auto switchManagerComponent = switchManager->AddComponent<DDM::MaterialSwitchManager>();
		switchManagerComponent->RegisterKey("Diffuse");
		switchManagerComponent->RegisterKey("Default");

		auto texturedMaterial = std::make_shared<DDM::TexturedMaterial>("DeferredDiffuse");
		
		texturedMaterial->AddTexture("resources/images/vehicle_diffuse.png");
		
		auto pVehicle{ DDM::DDMModelLoader::GetInstance().LoadModel("Resources/Models/vehicle.obj", scene->GetSceneRoot()) };
		pVehicle->SetShowImGui(true);
		
		auto pVehicleModel{ pVehicle->GetComponent<DDM::MeshRenderComponent>() };
		pVehicleModel->SetMaterial(texturedMaterial);
		

		auto defaultMaterial = std::make_shared<DDM::Material>();
		
		auto pVehicleTransform{ pVehicle->GetTransform() };
		pVehicleTransform->SetShowImGui(true);
		pVehicleTransform->SetLocalPosition(3, 3, 0);
		pVehicleTransform->SetLocalRotation(0.f, glm::radians(75.0f), 0.f);
		pVehicleTransform->SetLocalScale(0.5f, 0.5f, 0.5f);

		auto pMaterialSwitcher = pVehicle->AddComponent<DDM::MaterialSwitcher>();
		pMaterialSwitcher->RegisterMaterial("Diffuse", texturedMaterial);
		pMaterialSwitcher->RegisterMaterial("Default", defaultMaterial);

		switchManagerComponent->RegisterMaterialSwitcher(pMaterialSwitcher);
	}

	void SetupInfoComponent(DDM::Scene* scene)
	{
		auto pInfoObject{ scene->CreateGameObject("InfoComponent") };
		pInfoObject->SetShowImGui(true);

		auto pInfoComponent{ pInfoObject->AddComponent<DDM::InfoComponent>() };
		pInfoComponent->SetShowImGui(true);
	}

	void SetupCamera(DDM::Scene* scene)
	{
		auto pCamera{ scene->CreateGameObject("Camera") };

		pCamera->AddComponent<DDM::SpectatorMovement>();

		auto pCameraComponent{ pCamera->AddComponent<DDM::Camera>() };



		auto pCameraTransform{ pCamera->GetTransform() };
		pCameraTransform->SetLocalPosition(8.f, 1.5f, -0.3f);
		pCameraTransform->SetLocalRotation(0.0f, glm::radians(90.0f), 0.0f);

		//pCameraTransform->SetLocalPosition(3.82492f, 5.96203f, -3.80923);
		//pCameraTransform->SetLocalRotation(-2.73159, 0.0011386, 3.14159);

		//pCamera->AddComponent<D3D::RotatorComponent>();

		scene->SetCamera(pCameraComponent);
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
		pLightTransform->SetLocalRotation(glm::vec3(glm::radians(45.f), glm::radians(45.f), 0.0f));


		//pLight->AddComponent<D3D::RotatorComponent>();

		scene->SetLight(pLightComponent);
	}

	void SetupGroundPlane(DDM::Scene* scene)
	{
		std::shared_ptr<DDM::Material> pFloorMaterial{ std::make_shared<DDM::Material>() };


		//std::shared_ptr<D3D::TexturedMaterial> pFloorMaterial{
		//	std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"resources/images/GroundPlane.png"}, "DiffuseUnshaded") };


		auto pGroundPlane{ scene->CreateGameObject("Ground Plane") };
		pGroundPlane->SetShowImGui(true);

		auto pGroundplaneMesh{ DDM::ResourceManager::GetInstance().LoadMesh("Resources/Models/Plane.obj") };

		auto pGroundPlaneModel{ pGroundPlane->AddComponent<DDM::MeshRenderComponent>() };
		pGroundPlaneModel->SetShowImGui(true);
		pGroundPlaneModel->SetMesh(pGroundplaneMesh);
		pGroundPlaneModel->SetMaterial(pFloorMaterial);
	}

	void SetupGear(DDM::Scene* scene)
	{
		std::shared_ptr<DDM::Material> pGearMaterial{ std::make_shared<DDM::Material>() };


		//std::shared_ptr<D3D::TexturedMaterial> pFloorMaterial{
		//	std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"resources/images/GroundPlane.png"}, "DiffuseUnshaded") };


		auto pGear{ scene->CreateGameObject("Gear") };
		pGear->SetShowImGui(true);

		auto rotator = pGear->AddComponent<DDM::RotatorComponent>();
		rotator->SetRotSpeed(10.0f);

		auto pGearMesh{ DDM::ResourceManager::GetInstance().LoadMesh("Resources/Models/Gear.obj") };

		auto pGearModel{ pGear->AddComponent<DDM::MeshRenderComponent>() };
		pGearModel->SetShowImGui(true);
		pGearModel->SetMesh(pGearMesh);
		pGearModel->SetMaterial(pGearMaterial);

		auto pGearTransform{ pGear->GetTransform() };
		pGearTransform->SetShowImGui(true);
	}
}