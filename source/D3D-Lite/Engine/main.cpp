#include "D3DEngine.h"
#include "../Vulkan/VulkanRenderer.h"
#include "../Managers/SceneManager.h"

#include "../DataTypes/Materials/Material.h"
#include "../DataTypes/Materials/TexturedMaterial.h"

#include "../Components/MeshRenderComponent.h"
#include "../DataTypes/Mesh.h"
#include "../Components/TransformComponent.h"
#include "Scene.h"
#include "../Components/SkyBoxComponent.h"

#include "../Components/CameraComponent.h"
#include "../Components/DirectionalLightComponent.h"

#include "../Components/RotatorComponent.h"
#include "../Components/SpectatorMovementComponent.h"

#include "../Vulkan/VulkanRenderer.h"


void SetupPipelines();

void SetupVehicle(D3D::Scene* scene);

void SetupVikingRoom(D3D::Scene* scene);

void SetupCamera(D3D::Scene* scen);

void SetupLight(D3D::Scene* scene);

void load()
{
	auto scene = D3D::SceneManager::GetInstance().CreateScene("Test");
	D3D::SceneManager::GetInstance().SetActiveScene(scene);

	SetupPipelines();

	auto& renderer{ D3D::VulkanRenderer::GetInstance() };

	SetupVehicle(scene.get());

	SetupVikingRoom(scene.get());
	
	//auto pRobloxRigged{ std::make_shared<D3D::Mesh>("Resources/Models/RobloxRigged.obj") };
	//auto pRobloxObject{ scene->CreateGameObject("Roblox rig") };
	//auto pRobloxModel{ pRobloxObject->AddComponent<D3D::MeshRenderComponent>() };
	//pRobloxModel->SetMesh(pRobloxRigged);
	//pRobloxModel->SetMaterial(pVehicleMaterial);

	SetupCamera(scene.get());

	SetupLight(scene.get());
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
}

void SetupVehicle(D3D::Scene* scene)
{
	std::shared_ptr<D3D::TexturedMaterial> pVehicleMaterial{ std::make_shared<D3D::TexturedMaterial>
		(std::initializer_list<const std::string>{"resources/images/vehicle_diffuse.png", "resources/images/vehicle_normal.png",
		"resources/images/vehicle_gloss.png", "resources/images/vehicle_specular.png"},
			"DiffNormSpec") };

	auto pVehicle{ scene->CreateGameObject("Vehicle") };
	pVehicle->AddComponent<D3D::RotatorComponent>();

	auto pVehicleMesh{ std::make_shared<D3D::Mesh>("Resources/Models/vehicle.obj") };

	auto pVehicleModel{ pVehicle->AddComponent<D3D::MeshRenderComponent>() };
	pVehicleModel->SetMesh(pVehicleMesh);
	pVehicleModel->SetMaterial(pVehicleMaterial);


	auto pVehicleTransform{ pVehicle->GetTransform() };
	pVehicleTransform->SetLocalPosition(-1.f, 0, 3.f);
	pVehicleTransform->SetLocalRotation(0.f, glm::radians(75.0f), 0.f);
	pVehicleTransform->SetLocalScale(0.05f, 0.05f, 0.05f);
}

void SetupVikingRoom(D3D::Scene* scene)
{
	std::shared_ptr<D3D::TexturedMaterial> pVikingMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"resources/images/viking_room.png"}, "Diffuse") };
	
	auto pvikingRoom{ scene->CreateGameObject("Viking Room") };

	auto pVikingRoomMesh{ std::make_shared<D3D::Mesh>("Resources/Models/viking_room.obj") };

	auto pVikingRoomModel{ pvikingRoom->AddComponent<D3D::MeshRenderComponent>() };
	pVikingRoomModel->SetMesh(pVikingRoomMesh);
	pVikingRoomModel->SetMaterial(pVikingMaterial);

	auto pVikingTransform{ pvikingRoom->GetTransform() };
	pVikingTransform->SetLocalPosition(1.f, -0.2f, 3.f);
	pVikingTransform->SetLocalRotation(glm::radians(-90.0f), glm::radians(45.0f), 0.f);
	pVikingTransform->SetLocalScale(0.75f, 0.75f, 0.75f);
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

	auto pLightComponent{ pLight->AddComponent<D3D::DirectionalLightComponent>() };

	auto pLightTransform{ pLight->GetTransform() };

	pLightTransform->SetLocalRotation(glm::vec3(0.0f, glm::radians(180.f), 0.0f));

	pLight->AddComponent<D3D::RotatorComponent>();

	scene->SetLight(pLightComponent);
}

int main()
{
	// Create the engine object and run it with the load function
	D3D::D3DEngine engine{};
	engine.Run(load);

	return EXIT_SUCCESS;
} 