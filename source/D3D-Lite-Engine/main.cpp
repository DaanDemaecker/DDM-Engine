#include "D3DEngine.h"
#include "VulkanRenderer.h"
#include "SceneManager.h"

#include "Material.h"
#include "TexturedMaterial.h"

#include "ModelComponent.h"
#include "TransformComponent.h"
#include "Scene.h"

#include "CameraComponent.h"

#include "RotatorComponent.h"

#include "VulkanRenderer.h"

void load()
{
	auto scene = D3D::SceneManager::GetInstance().CreateScene("Test");
	D3D::SceneManager::GetInstance().SetActiveScene(scene);

	auto& renderer{ D3D::VulkanRenderer::GetInstance() };

	renderer.AddGraphicsPipeline("Diffuse", "../Resources/Shaders/Diffuse.Vert.spv", "../Resources/Shaders/Diffuse.Frag.spv", 1, 1, 1);

	std::shared_ptr<D3D::TexturedMaterial> pVikingMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"../resources/images/viking_room.png"}, "Diffuse") };
	std::shared_ptr<D3D::TexturedMaterial> pVehicleMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"../resources/images/vehicle_diffuse.png"}, "Diffuse") };

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


	auto pCamera{ scene->CreateGameObject("Camera") };

	auto pCameraComponent{ pCamera->AddComponent<D3D::CameraComponent>() };

	auto pCameraTransform{ pCamera->GetTransform() };
	pCameraTransform->SetLocalRotation(glm::vec3(0.0f, glm::radians(180.f), 0.0f));

	scene->SetCamera(pCameraComponent);
}

int main()
{

	D3D::D3DEngine engine{};
	engine.Run(load);

	return EXIT_SUCCESS;
} 