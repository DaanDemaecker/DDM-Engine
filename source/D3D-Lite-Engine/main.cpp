#include "D3DEngine.h"
#include "VulkanRenderer.h"
#include "SceneManager.h"

#include "Material.h"
#include "TexturedMaterial.h"

#include "ModelComponent.h"
#include "TransformComponent.h"
#include "Scene.h"

#include "CameraComponent.h"
#include "DirectionalLightComponent.h"

#include "RotatorComponent.h"
#include "SpectatorMovementComponent.h"

#include "VulkanRenderer.h"

void load()
{
	auto scene = D3D::SceneManager::GetInstance().CreateScene("Test");
	D3D::SceneManager::GetInstance().SetActiveScene(scene);

	auto& renderer{ D3D::VulkanRenderer::GetInstance() };

	renderer.AddGraphicsPipeline("Diffuse", { "../Resources/Shaders/Diffuse.Vert.spv", "../Resources/Shaders/Diffuse.Frag.spv" });
	renderer.AddGraphicsPipeline("NormalMap", { "../Resources/Shaders/NormalMap.Vert.spv", "../Resources/Shaders/NormalMap.Frag.spv" });
	renderer.AddGraphicsPipeline("DiffNorm", { "../Resources/Shaders/DiffNorm.Vert.spv", "../Resources/Shaders/DiffNorm.Frag.spv" });
	renderer.AddGraphicsPipeline("Test", { "../Resources/Shaders/Test.Vert.spv", "../Resources/Shaders/Test.Frag.spv" });
	renderer.AddGraphicsPipeline("DiffuseUnshaded", { "../Resources/Shaders/DiffuseUnshaded.Vert.spv", "../Resources/Shaders/DiffuseUnshaded.Frag.spv" });
	renderer.AddGraphicsPipeline("Specular", { "../Resources/Shaders/Specular.Vert.spv", "../Resources/Shaders/Specular.Frag.spv" });
	renderer.AddGraphicsPipeline("DiffNormSpec", { "../Resources/Shaders/DiffNormSpec.Vert.spv", "../Resources/Shaders/DiffNormSpec.Frag.spv" });

	std::shared_ptr<D3D::TexturedMaterial> pVikingMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"../resources/images/viking_room.png"}, "Diffuse") };
	std::shared_ptr<D3D::TexturedMaterial> pVehicleMaterial{ std::make_shared<D3D::TexturedMaterial>
		(std::initializer_list<const std::string>{"../resources/images/vehicle_diffuse.png", "../resources/images/vehicle_normal.png",
		"../resources/images/vehicle_gloss.png", "../resources/images/vehicle_specular.png"},
			"DiffNormSpec") };
	auto pvikingRoom{ scene->CreateGameObject("Viking Room") };

	auto pVikingRoomModel{ pvikingRoom->AddComponent<D3D::ModelComponent>() };
	pVikingRoomModel->LoadModel("../Resources/Models/viking_room.obj");
	pVikingRoomModel->SetMaterial(pVikingMaterial);



	auto pVikingTransform{ pvikingRoom->GetTransform() };
	pVikingTransform->SetLocalPosition(1.f, -0.2f, 3.f);
	pVikingTransform->SetLocalRotation(glm::radians(-90.0f), glm::radians(225.0f), 0.f);
	pVikingTransform->SetLocalScale(0.75f, 0.75f, 0.75f);

	auto pVehicle{ scene->CreateGameObject("Vehicle") };
	pVehicle->AddComponent<D3D::RotatorComponent>();

	auto pVehicleModel{ pVehicle->AddComponent<D3D::ModelComponent>() };
	pVehicleModel->LoadModel("../Resources/Models/vehicle.obj");
	pVehicleModel->SetMaterial(pVehicleMaterial);


	auto pVehicleTransform{ pVehicle->GetTransform() };
	pVehicleTransform->SetLocalPosition(-1.f, 0, 3.f);
	pVehicleTransform->SetLocalRotation(0.f, glm::radians(75.0f), 0.f);
	pVehicleTransform->SetLocalScale(0.05f, 0.05f, 0.05f);


	auto pCamera{ scene->CreateGameObject("Camera") };

	pCamera->AddComponent<D3D::SpectatorMovementComponent>();

	auto pCameraComponent{ pCamera->AddComponent<D3D::CameraComponent>() };

	auto pCameraTransform{ pCamera->GetTransform() };
	//pCameraTransform->SetLocalRotation(glm::vec3(0.0f, glm::radians(180.f), 0.0f));

	//pCamera->AddComponent<D3D::RotatorComponent>();

	scene->SetCamera(pCameraComponent);


	auto pLight{ scene->CreateGameObject("Light") };

	auto pLightComponent{ pLight->AddComponent<D3D::DirectionalLightComponent>() };

	auto pLightTransform{ pLight->GetTransform() };

	pLightTransform->SetLocalRotation(glm::vec3(0.0f, glm::radians(180.f), 0.0f));

	pLight->AddComponent<D3D::RotatorComponent>();

	scene->SetLight(pLightComponent);
	
}

int main()
{

	D3D::D3DEngine engine{};
	engine.Run(load);

	return EXIT_SUCCESS;
} 