#include "stdafx.h"
#include "Scene.h"
#include "GameObject.h"


unsigned int D3D::Scene::m_IdCounter = 0;



D3D::Scene::Scene(const std::string& name) : m_Name(name)
{
	m_pSceneRoot = std::make_unique<GameObject>("Scene Root");
	m_pSceneRoot->Init();
}

D3D::GameObject* D3D::Scene::CreateGameObject(const std::string& name)
{
	return m_pSceneRoot->CreateNewObject(name);
}

void D3D::Scene::OnSceneLoad()
{
	
}

void D3D::Scene::OnSceneUnload()
{
	m_pSceneRoot->OnSceneLoad();
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
