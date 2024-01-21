#include "Scene.h"
#include "GameObject.h"

#include "CameraComponent.h"

unsigned int D3D::Scene::m_IdCounter = 0;



D3D::Scene::Scene(const std::string& name) : m_Name(name)
{
	m_pSceneRoot = std::make_unique<GameObject>("Scene Root");
	m_pSceneRoot->Init();

	m_pDefaultCamera = std::make_unique<GameObject>("Default Camera");
	m_pDefaultCamera->Init();
	m_pDefaultCameraComponent = m_pDefaultCamera->AddComponent<CameraComponent>();
}

D3D::GameObject* D3D::Scene::CreateGameObject(const std::string& name)
{
	return m_pSceneRoot->CreateNewObject(name);
}

void D3D::Scene::OnSceneLoad()
{
	m_pSceneRoot->OnSceneLoad();
}

void D3D::Scene::OnSceneUnload()
{
	m_pSceneRoot->OnSceneUnload();
}

void D3D::Scene::StartFrame()
{
	m_pSceneRoot->StartFrame();
}

void D3D::Scene::Update()
{
	m_pSceneRoot->Update();
}

void D3D::Scene::FixedUpdate()
{
	m_pSceneRoot->FixedUpdate();
}

void D3D::Scene::LateUpdate()
{
	m_pSceneRoot->LateUpdate();
}

void D3D::Scene::Render() const
{
	m_pSceneRoot->Render();
}

void D3D::Scene::OngGUI() const
{
	m_pSceneRoot->OnGUI();
}

void D3D::Scene::Cleanup()
{
	m_pSceneRoot->Cleanup();
}

void D3D::Scene::SetCamera(std::shared_ptr<CameraComponent> pCamera)
{
	m_pActiveCamera = pCamera;
}

const std::shared_ptr<D3D::CameraComponent> D3D::Scene::GetCamera() const
{
	if (m_pActiveCamera != nullptr)
		return m_pActiveCamera;

	return m_pDefaultCameraComponent;
}
