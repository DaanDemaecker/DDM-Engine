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

#include "Vulkan/Renderers/AORenderers/HBAORenderer.h"

namespace LoadHBAOScene
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

		vulkanObject.AddGraphicsPipeline("DeferredDiffuse", { "Resources/Shaders/SSAO/SSAOGbuffer.Vert.spv", "Resources/Shaders/SSAO/SSAODiffuse.frag.spv" }, true, false, DDM3::HBAORenderer::kSubpass_GBUFFER);
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
		pCameraTransform->SetLocalPosition(0, 1, 0);

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