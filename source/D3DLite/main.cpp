#include "stdafx.h"

#include "D3DEngine.h"
#include "VulkanRenderer.h"
#include "SceneManager.h"

#include "Material.h"
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "Scene.h"

void load()
{
	auto scene = D3D::SceneManager::GetInstance().CreateScene("Test");
	D3D::SceneManager::GetInstance().SetActiveScene(scene);

	std::shared_ptr<D3D::Material> pVikingMaterial{ std::make_shared<D3D::Material>("../resources/images/viking_room.png") };
	std::shared_ptr<D3D::Material> pVehicleMaterial{ std::make_shared<D3D::Material>("../resources/images/vehicle_diffuse.png") };

	auto pvikingRoom{ scene->CreateGameObject("Viking Room") };

	auto pVikingRoomModel{ pvikingRoom->AddComponent<D3D::ModelComponent>() };
	pVikingRoomModel->LoadModel("../Resources/Models/viking_room.obj");
	pVikingRoomModel->SetMaterial(pVikingMaterial);



	auto pVikingTransform{ pvikingRoom->GetTransform() };
	pVikingTransform->SetLocalPosition(1.f, -0.2f, 5.f);
	pVikingTransform->SetLocalRotation(glm::radians(-90.0f), glm::radians(45.0f), 0.f);
	pVikingTransform->SetLocalScale(0.75f, 0.75f, 0.75f);


	auto pVehicle{ scene->CreateGameObject("Vehicle") };

	auto pVehicleModel{ pVehicle->AddComponent<D3D::ModelComponent>() };
	pVehicleModel->LoadModel("../Resources/Models/vehicle.obj");
	pVehicleModel->SetMaterial(pVehicleMaterial);


	auto pVehicleTransform{ pVehicle->GetTransform() };
	pVehicleTransform->SetLocalPosition(-1.f, 0, 5.f);
	pVehicleTransform->SetLocalRotation(0.f, glm::radians(75.0f), 0.f);
	pVehicleTransform->SetLocalScale(0.05f, 0.05f, 0.05f);
}

int main()
{
	const uint32_t windowWidth = 800;
	const uint32_t windowHeight = 600;

	D3D::D3DEngine engine(static_cast<int>(windowWidth), static_cast<int>(windowHeight));
	engine.Run(load);

	return EXIT_SUCCESS;
} 