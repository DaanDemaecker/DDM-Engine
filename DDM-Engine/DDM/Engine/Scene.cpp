// Scene.cpp

// Header include
#include "Scene.h"

// File includes
#include "BaseClasses/GameObject.h"

#include "Components/Camera.h"
#include "Components/Light/LightComponent.h"
#include "Components/Transform.h"

unsigned int DDM::Scene::m_IdCounter = 0;

DDM::Scene::Scene(const std::string& name) : m_Name(name)
{
	m_pSceneRoot = std::make_unique<GameObject>("Scene Root");
	m_pSceneRoot->Init();

	m_pDefaultCamera = std::make_unique<GameObject>("Default Camera");
	m_pDefaultCamera->Init();
	m_pDefaultCameraComponent = m_pDefaultCamera->AddComponent<Camera>();

	m_pDefaultLight = std::make_unique<GameObject>("Default Light");
	m_pDefaultLight->Init();
	m_pDefaultLightComponent = m_pDefaultLight->AddComponent<LightComponent>();
}

DDM::GameObject* DDM::Scene::CreateGameObject(const std::string& name)
{
	return m_pSceneRoot->CreateNewObject(name);
}

void DDM::Scene::OnSceneLoad()
{
	m_pSceneRoot->OnSceneLoad();
}

void DDM::Scene::OnSceneUnload()
{
	m_pSceneRoot->OnSceneUnload();
}

void DDM::Scene::StartFrame()
{
	m_pSceneRoot->StartFrame();
}

void DDM::Scene::EarlyUpdate()
{
	m_pSceneRoot->EarlyUpdate();
}

void DDM::Scene::Update()
{
	m_pSceneRoot->Update();
}

void DDM::Scene::FixedUpdate()
{
	m_pSceneRoot->FixedUpdate();
}

void DDM::Scene::LateUpdate()
{
	m_pSceneRoot->LateUpdate();
}

void DDM::Scene::PostUpdate()
{
	m_pSceneRoot->PostUpdate();
}

void DDM::Scene::RenderSkyBox()
{
	if (m_pActiveCamera != nullptr)
		m_pActiveCamera->RenderSkybox();
}

void DDM::Scene::RenderDepth() const
{
	m_pSceneRoot->RenderDepth();
}

void DDM::Scene::Render() const
{
	m_pSceneRoot->Render();
}

void DDM::Scene::RenderTransparancy() const
{
	m_pSceneRoot->RenderTransparancy();
}

void DDM::Scene::OngGUI() const
{
	m_pSceneRoot->OnGUI();
}

void DDM::Scene::SetCamera(std::shared_ptr<Camera> pCamera)
{
	m_pActiveCamera = pCamera;
}

const std::shared_ptr<DDM::Camera> DDM::Scene::GetCamera() const
{
	if (m_pActiveCamera != nullptr)
		return m_pActiveCamera;

	return m_pDefaultCameraComponent;
}

void DDM::Scene::SetLight(std::shared_ptr<LightComponent> pLight)
{
	m_pActiveLight = pLight;
}

const std::shared_ptr<DDM::LightComponent> DDM::Scene::GetLight() const
{
	if (m_pActiveLight != nullptr)
		return m_pActiveLight;

	return m_pDefaultLightComponent;
}

DDM::GameObject* DDM::Scene::GetSceneRoot()
{
	return m_pSceneRoot.get();
}

