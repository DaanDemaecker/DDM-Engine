// Skybox.cpp

// Header include
#include "SkyBox.h"

// File includes
#include "Utils/Utils.h"

#include "Vulkan/VulkanObject.h"

#include "DataTypes/Mesh.h"
#include "DataTypes/DescriptorObjects/TextureDescriptorObject.h"	
#include "DataTypes/Materials/CubeMapMaterial.h"

#include "Components/TransformComponent.h"

#include "Managers/ResourceManager.h"
#include "Managers/ConfigManager.h"

DDM::SkyBoxComponent::SkyBoxComponent()
{
	// Create the model
	auto pMesh = ResourceManager::GetInstance().LoadMesh(ConfigManager::GetInstance().GetString("SkyboxModel"));

	// Set the mesh in the meshrenderer
	SetMesh(pMesh);
}

void DDM::SkyBoxComponent::EarlyUpdate()
{
	// Create descriptorsets if necesarry
	if (m_ShouldCreateDescriptorSets)
	{
		CreateDescriptorSets();
		m_ShouldCreateDescriptorSets = false;
	}
}

void DDM::SkyBoxComponent::LoadSkybox(std::initializer_list<const std::string>&& filePaths)
{
	// Create cubemap material
	m_pMaterial = std::make_shared<DDM::CubeMapMaterial>(filePaths);
}

void DDM::SkyBoxComponent::RenderSkyBox()
{
	// If no mesh, return
	if (m_pMesh == nullptr)
		return;

	// Get index of current frame in flight
	auto frame{ DDM::VulkanObject::GetInstance().GetCurrentFrame()};

	// Update uniform buffer
	UpdateUniformBuffer(frame);

	// Render pipeline
	m_pMesh->Render(GetPipeline(), &m_DescriptorSets[frame]);
}

void DDM::SkyBoxComponent::Render()
{
	// Empty function to avoid rendering in the normal render pass
}
