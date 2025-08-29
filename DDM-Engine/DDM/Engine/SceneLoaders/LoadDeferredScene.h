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
#include "../../Components/SpectatorMovementComponent.h"

#include "../../Managers/ResourceManager.h"
#include "../../Managers/ConfigManager.h"
#include "Engine/DDMModelLoader.h"

#include "Components/MaterialSwitcher/MaterialSwitchManager.h"
#include "Components/MaterialSwitcher/MaterialSwitcher.h"
#include "Components/InfoComponent.h"

#include "Vulkan/Renderers/DeferredRenderer.h"

namespace LoadDeferredScene
{
	void SetupPipelines();

	void SetupVehicle(DDM::Scene* scene);

	void SetupVehicle2(DDM::Scene* scene);

	void SetupRoom(DDM::Scene* scene);

	void SetupVikingRoom(DDM::Scene* scene);

	void SetupGun(DDM::Scene* scene);

	void SetupMario(DDM::Scene* scene);

	void SetupAtrium(DDM::Scene* scene);

	void SetupAtrium2(DDM::Scene* scene);

	void SetupAtrium3(DDM::Scene* scene);

	void SetupSkull(DDM::Scene* scene);

	void SetupInfoComponent(DDM::Scene* scene);

	void SetupCamera(DDM::Scene* scen);

	void SetupLight(DDM::Scene* scene);

	void SetupGroundPlane(DDM::Scene* scene);

	void LoadScene()
	{
		auto scene = DDM::SceneManager::GetInstance().CreateScene("Test");
		DDM::SceneManager::GetInstance().SetActiveScene(scene);

		SetupPipelines();

		//SetupGroundPlane(scene.get());

		//SetupVehicle(scene.get());

		//SetupVehicle2(scene.get());

		//SetupVikingRoom(scene.get());

		//SetupGun(scene.get());

		//SetupMario(scene.get());

		//SetupAtrium(scene.get());

		//SetupAtrium2(scene.get());

		SetupAtrium3(scene.get());

		//SetupRoom(scene.get());

		//SetupSkull(scene.get());

		SetupInfoComponent(scene.get());

		SetupCamera(scene.get());

		SetupLight(scene.get());
	}

	void SetupPipelines()
	{
		auto& vulkanObject{ DDM::VulkanObject::GetInstance() };

		vulkanObject.AddGraphicsPipeline("DeferredDiffuse", { "Resources/DefaultResources/Deffered.Vert.spv", "Resources/Shaders/DefferedDiffuse.frag.spv" }, true, false, DDM::DeferredRenderer::kSubpass_GBUFFER);
	}

	void SetupVehicle(DDM::Scene* scene)
	{
		std::shared_ptr<DDM::MultiMaterial> pVehicleMaterial{ std::make_shared<DDM::MultiMaterial>() };

		pVehicleMaterial->AddDiffuseTexture("resources/images/vehicle_diffuse.png");

		pVehicleMaterial->AddNormalMap("resources/images/vehicle_normal.png");

		pVehicleMaterial->AddGlossTextures(std::initializer_list<const std::string>{"resources/images/vehicle_gloss.png"});

		pVehicleMaterial->AddSpecularTextures(std::initializer_list<const std::string>{"resources/images/vehicle_specular.png"});

		auto pVehicle{ scene->CreateGameObject("Vehicle") };
		pVehicle->SetShowImGui(true);
		//pVehicle->AddComponent<D3D::RotatorComponent>();

		auto pVehicleMesh{ DDM::ResourceManager::GetInstance().LoadMesh("Resources/Models/vehicle.obj") };

		auto pVehicleModel{ pVehicle->AddComponent<DDM::MeshRenderComponent>() };
		pVehicleModel->SetMesh(pVehicleMesh);
		pVehicleModel->SetMaterial(pVehicleMaterial);


		auto pVehicleTransform{ pVehicle->GetTransform() };
		pVehicleTransform->SetShowImGui(true);
		pVehicleTransform->SetLocalPosition(0, 3, 0);
		pVehicleTransform->SetLocalRotation(0.f, glm::radians(75.0f), 0.f);
		pVehicleTransform->SetLocalScale(0.05f, 0.05f, 0.05f);
	}

	void SetupVehicle2(DDM::Scene* scene)
	{
		std::shared_ptr<DDM::TexturedMaterial> pVehicleMaterial{ std::make_shared<DDM::TexturedMaterial>("DeferredDiffuse")};

		pVehicleMaterial->AddTexture("resources/images/vehicle_diffuse.png");

		auto pVehicle{ DDM::DDMModelLoader::GetInstance().LoadModel("Resources/Models/vehicle.obj", scene->GetSceneRoot()) };
		pVehicle->SetShowImGui(true);
		//pVehicle->AddComponent<D3D::RotatorComponent>();

		auto pVehicleModel{ pVehicle->GetComponent<DDM::MeshRenderComponent>() };
		pVehicleModel->SetMaterial(pVehicleMaterial);


		auto pVehicleTransform{ pVehicle->GetTransform() };
		pVehicleTransform->SetShowImGui(true);
		pVehicleTransform->SetLocalPosition(3, 3, 0);
		pVehicleTransform->SetLocalRotation(0.f, glm::radians(75.0f), 0.f);
		pVehicleTransform->SetLocalScale(0.05f, 0.05f, 0.05f);
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
			pRoomTransform->SetShowImGui(true);
			pRoomTransform->SetLocalScale(0.05f, 0.05f, 0.05f);
		}
	}

	void SetupVikingRoom(DDM::Scene* scene)
	{
		std::shared_ptr<DDM::TexturedMaterial> pVikingMaterial{ std::make_shared<DDM::TexturedMaterial>(std::initializer_list<const std::string>{"resources/images/viking_room.png"}, "Diffuse") };

		auto pvikingRoom{ scene->CreateGameObject("Viking Room") };

		auto pVikingRoomMesh{ DDM::ResourceManager::GetInstance().LoadMesh("Resources/Models/viking_room.obj") };

		auto pVikingRoomModel{ pvikingRoom->AddComponent<DDM::MeshRenderComponent>() };
		pVikingRoomModel->SetMesh(pVikingRoomMesh);
		pVikingRoomModel->SetMaterial(pVikingMaterial);

		auto pVikingTransform{ pvikingRoom->GetTransform() };
		pVikingTransform->SetLocalPosition(1.f, -0.2f, 3.f);
		pVikingTransform->SetLocalRotation(glm::radians(-90.0f), glm::radians(45.0f), 0.f);
		pVikingTransform->SetLocalScale(0.75f, 0.75f, 0.75f);
	}

	void SetupGun(DDM::Scene* scene)
	{
		std::shared_ptr<DDM::MultiMaterial> pGunMaterial{ std::make_shared<DDM::MultiMaterial>() };

		pGunMaterial->AddDiffuseTexture("resources/images/gun_BaseColor.png");

		pGunMaterial->AddNormalMap("resources/images/gun_Normal.png");

		auto pGun{ scene->CreateGameObject("Gun") };

		auto pGunMesh{ DDM::ResourceManager::GetInstance().LoadMesh("Resources/Models/gun.fbx") };

		auto pGunModel{ pGun->AddComponent<DDM::MeshRenderComponent>() };
		pGunModel->SetMesh(pGunMesh);
		pGunModel->SetMaterial(pGunMaterial);

		auto pGunTransform{ pGun->GetTransform() };
		pGunTransform->SetLocalPosition(0.f, -.5f, 6.f);
		pGunTransform->SetLocalRotation(0.f, glm::radians(-90.f), 0.f);
		pGunTransform->SetLocalScale(0.5f, 0.5f, 0.5f);
	}

	void SetupMario(DDM::Scene* scene)
	{
		auto pMario = DDM::DDMModelLoader::GetInstance().LoadModel("Resources/Models/MarioDancing.fbx", scene->GetSceneRoot());

		//auto pMarioTransform{ pMario->GetTransform() };
		//pMarioTransform->SetLocalPosition(0.f, 0.f, 2);
		//pMarioTransform->SetLocalRotation(0.f, glm::radians(180.f), 0.f);
		//pMarioTransform->SetLocalScale(0.5f, 0.5f, 0.5f);
	}


	void SetupAtrium(DDM::Scene* scene)
	{
		DDM::DDMModelLoader::GetInstance().LoadTexturedScene("Resources/Models/SponzaAtrium/Sponza.gltf", scene->GetSceneRoot());
	}

	void SetupAtrium2(DDM::Scene* scene)
	{
		auto& modelLoader = DDM::DDMModelLoader::GetInstance();

		std::vector<std::unique_ptr<DDMML::Mesh>> pMeshes{};

		modelLoader.LoadScene("Resources/Models/SponzaAtrium/Sponza.gltf", pMeshes);

		auto sceneRoot = scene->GetSceneRoot();

		for (auto& pMesh : pMeshes)
		{
			auto pGameObject = scene->GetSceneRoot()->CreateNewObject(pMesh->GetName());
			auto renderComponent = pGameObject->AddComponent<DDM::MeshRenderComponent>();
			renderComponent->SetMesh(pMesh.get());


			auto texturedMaterial = std::make_shared<DDM::TexturedMaterial>("Default");

			for (auto& texture : pMesh->GetDiffuseTextureNames())
			{
				texturedMaterial->AddTexture(texture);
			}

			renderComponent->SetMaterial(texturedMaterial);
		}
	}

	void SetupAtrium3(DDM::Scene* scene)
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

	void SetupSkull(DDM::Scene* scene)
	{
		auto switchManager = scene->GetSceneRoot()->CreateNewObject("Material switch manager");
		switchManager->SetShowImGui(true);

		auto switchManagerComponent = switchManager->AddComponent<DDM::MaterialSwitchManager>();
		switchManagerComponent->RegisterKey("Diffuse");
		switchManagerComponent->RegisterKey("Default");

		auto pMesh = std::unique_ptr<DDMML::Mesh>(std::make_unique<DDMML::Mesh>());

		DDM::DDMModelLoader::GetInstance().LoadModel("Resources/Models/Skull/Scene.gltf", pMesh);

		auto pGameObject = scene->GetSceneRoot()->CreateNewObject(pMesh->GetName());
		auto renderComponent = pGameObject->AddComponent<DDM::MeshRenderComponent>();
		renderComponent->SetMesh(pMesh.get());


		auto texturedMaterial = std::make_shared<DDM::TexturedMaterial>("Diffuse");

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

		pCamera->AddComponent<DDM::SpectatorMovementComponent>();

		auto pCameraComponent{ pCamera->AddComponent<DDM::Camera>() };


		auto pCameraTransform{ pCamera->GetTransform() };
		pCameraTransform->SetLocalPosition(8.f, 1.5f, -0.3f);
		pCameraTransform->SetLocalRotation(0.0f, glm::radians(90.0f), 0.0f);

		//pCameraTransform->SetLocalPosition(3.82492f, 5.96203f, -3.80923);
		//pCameraTransform->SetLocalRotation(-2.73159, 0.0011386, 3.14159);

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

	void SetupLight(DDM::Scene* scene)
	{
		auto pLight{ scene->CreateGameObject("Light") };
		pLight->SetShowImGui(true);

		auto pLightComponent{ pLight->AddComponent<DDM::LightComponent>() };
		pLightComponent->SetShowImGui(true);

		//pLightComponent->SetColor(glm::vec3{ 0, 0, 0 });pp

		auto pLightTransform{ pLight->GetTransform() };
		pLightTransform->SetShowImGui(true);
		pLightTransform->SetLocalRotation(glm::vec3(glm::radians(45.f), glm::radians(45.f), 0.0f));


		//pLight->AddComponent<D3D::RotatorComponent>();

		scene->SetLight(pLightComponent);
	}

	void SetupGroundPlane(DDM::Scene* scene)
	{
		std::shared_ptr<DDM::MultiMaterial> pFloorMaterial{ std::make_shared<DDM::MultiMaterial>() };


		//std::shared_ptr<D3D::TexturedMaterial> pFloorMaterial{
		//	std::make_shared<D3D::TexturedMaterial>(std::initializer_lpist<const std::string>{"resources/images/GroundPlane.png"}, "DiffuseUnshaded") };

		pFloorMaterial->AddDiffuseTexture("resources/images/GroundPlane.png");

		auto pGroundPlane{ scene->CreateGameObject("Ground Plane") };
		pGroundPlane->SetShowImGui(true);

		auto pGroundplaneMesh{ DDM::ResourceManager::GetInstance().LoadMesh("Resources/Models/Plane.obj") };

		auto pGroundPlaneModel{ pGroundPlane->AddComponent<DDM::MeshRenderComponent>() };
		pGroundPlaneModel->SetShowImGui(true);
		pGroundPlaneModel->SetMesh(pGroundplaneMesh);
		pGroundPlaneModel->SetMaterial(pFloorMaterial);
	}
}