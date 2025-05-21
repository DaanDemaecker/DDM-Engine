// Skybox.cpp

// File includes
#include "SkyBoxComponent.h"
#include "../Utils/Utils.h"
#include "Vulkan/VulkanObject.h"
#include "../DataTypes/Mesh.h"
#include "../DataTypes/DescriptorObjects/TextureDescriptorObject.h"	
#include "../DataTypes/Materials/CubeMapMaterial.h"
#include "../Managers/ResourceManager.h"
#include "TransformComponent.h"
#include "../Managers/ConfigManager.h"

DDM3::SkyBoxComponent::SkyBoxComponent()
{
	// Create the model
	auto pMesh = ResourceManager::GetInstance().LoadMesh(ConfigManager::GetInstance().GetString("SkyboxModel"));

	SetMesh(pMesh);
}

void DDM3::SkyBoxComponent::EarlyUpdate()
{
	if (m_ShouldCreateDescriptorSets)
	{
		CreateDescriptorSets();
		m_ShouldCreateDescriptorSets = false;
	}
}

void DDM3::SkyBoxComponent::LoadSkybox(std::initializer_list<const std::string>&& filePaths)
{
	// Create cubemap material
	m_pMaterial = std::make_shared<DDM3::CubeMapMaterial>(filePaths);
}

void DDM3::SkyBoxComponent::RenderSkyBox()
{
	if (m_pMesh == nullptr)
		return;

	auto frame{ DDM3::VulkanObject::GetInstance().GetCurrentFrame()};

	UpdateUniformBuffer(frame);

	m_pMesh->Render(GetPipeline(), &m_DescriptorSets[frame]);
}

void DDM3::SkyBoxComponent::Render()
{
}
