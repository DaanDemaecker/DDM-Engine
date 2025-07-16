// Scene.cpp

// Header include
#include "Scene.h"

// File includes
#include "BaseClasses/GameObject.h"

#include "Components/CameraComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/TransformComponent.h"

unsigned int DDM3::Scene::m_IdCounter = 0;

DDM3::Scene::Scene(const std::string& name) : m_Name(name)
{
	m_pSceneRoot = std::make_unique<GameObject>("Scene Root");
	m_pSceneRoot->Init();

	m_pDefaultCamera = std::make_unique<GameObject>("Default Camera");
	m_pDefaultCamera->Init();
	m_pDefaultCameraComponent = m_pDefaultCamera->AddComponent<CameraComponent>();

	m_pDefaultLight = std::make_unique<GameObject>("Default Light");
	m_pDefaultLight->Init();
	m_pDefaultLightComponent = m_pDefaultLight->AddComponent<DirectionalLightComponent>();
}

DDM3::GameObject* DDM3::Scene::CreateGameObject(const std::string& name)
{
	return m_pSceneRoot->CreateNewObject(name);
}

void DDM3::Scene::OnSceneLoad()
{
	m_pSceneRoot->OnSceneLoad();
}

void DDM3::Scene::OnSceneUnload()
{
	m_pSceneRoot->OnSceneUnload();
}

void DDM3::Scene::StartFrame()
{
	m_pSceneRoot->StartFrame();
}

void DDM3::Scene::EarlyUpdate()
{
	m_pSceneRoot->EarlyUpdate();
}

void DDM3::Scene::Update()
{
	m_pSceneRoot->Update();
}

void DDM3::Scene::FixedUpdate()
{
	m_pSceneRoot->FixedUpdate();
}

void DDM3::Scene::LateUpdate()
{
	m_pSceneRoot->LateUpdate();
}

void DDM3::Scene::PostUpdate()
{
	m_pSceneRoot->PostUpdate();
}

void DDM3::Scene::PrepareRender()
{
	m_pSceneRoot->PrepareRender();
}

void DDM3::Scene::RenderSkyBox()
{
	if (m_pActiveCamera != nullptr)
		m_pActiveCamera->RenderSkybox();
}

void DDM3::Scene::RenderDepth() const
{
	m_pSceneRoot->RenderDepth();
}

void DDM3::Scene::Render() const
{
	m_pSceneRoot->Render();
}

void DDM3::Scene::RenderTransparancy() const
{
	m_pSceneRoot->RenderTransparancy();
}

void DDM3::Scene::OngGUI() const
{
	m_pSceneRoot->OnGUI();
}

void DDM3::Scene::Cleanup()
{
	m_pSceneRoot->Cleanup();
}

void DDM3::Scene::SetCamera(std::shared_ptr<CameraComponent> pCamera)
{
	m_pActiveCamera = pCamera;
}

const std::shared_ptr<DDM3::CameraComponent> DDM3::Scene::GetCamera() const
{
	if (m_pActiveCamera != nullptr)
		return m_pActiveCamera;

	return m_pDefaultCameraComponent;
}

void DDM3::Scene::SetLight(std::shared_ptr<DirectionalLightComponent> pLight)
{
	m_pActiveLight = pLight;
}

const std::shared_ptr<DDM3::DirectionalLightComponent> DDM3::Scene::GetLight() const
{
	if (m_pActiveLight != nullptr)
		return m_pActiveLight;

	return m_pDefaultLightComponent;
}

DDM3::GameObject* DDM3::Scene::GetSceneRoot()
{
	return m_pSceneRoot.get();
}

