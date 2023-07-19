#include "stdafx.h"
#include "Scene.h"

#include "Material.h"
#include "Model.h"

unsigned int D3D::Scene::m_IdCounter = 0;



D3D::Scene::Scene(const std::string& name) : m_Name(name)
{
}

void D3D::Scene::OnSceneLoad()
{
	std::shared_ptr<D3D::Material> pVikingMaterial{ std::make_shared<D3D::Material>("../resources/images/viking_room.png") };
	std::shared_ptr<D3D::Material> pVehicleMaterial{ std::make_shared<D3D::Material>("../resources/images/vehicle_diffuse.png") };


	m_pModels.push_back(std::make_unique<Model>());
	m_pModels[0]->LoadModel("../Resources/Models/viking_room.obj");
	m_pModels[0]->SetMaterial(pVikingMaterial);
	m_pModels[0]->SetPosition(1.f, -0.2f, 5.f);
	m_pModels[0]->SetRotation(glm::radians(-90.0f), glm::radians(45.0f), 0.f);
	m_pModels[0]->SetScale(0.75f, 0.75f, 0.75f);

	m_pModels.push_back(std::make_unique<Model>());
	m_pModels[1]->LoadModel("../Resources/Models/vehicle.obj");
	m_pModels[1]->SetMaterial(pVehicleMaterial);
	m_pModels[1]->SetPosition(-1.f, 0, 5.f);
	m_pModels[1]->SetRotation(0.f, glm::radians(75.0f), 0.f);
	m_pModels[1]->SetScale(0.05f, 0.05f, 0.05f);
}

void D3D::Scene::OnSceneUnload()
{
}

void D3D::Scene::StartFrame()
{
	
}

void D3D::Scene::Update()
{
	
}

void D3D::Scene::FixedUpdate()
{
	
}

void D3D::Scene::LateUpdate()
{
	
}

void D3D::Scene::Render() const
{
	for (auto& pModel : m_pModels)
	{
		pModel->Render();
	}
}

void D3D::Scene::OngGUI() const
{

}

void D3D::Scene::Cleanup()
{
	
}
