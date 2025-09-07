// Skybox.cpp

// Header include
#include "SkyBox.h"

// File includes
#include "Utils/Utils.h"

#include "Vulkan/VulkanObject.h"

#include "DataTypes/Mesh.h"
#include "DataTypes/DescriptorObjects/TextureDescriptorObject.h"	
#include "DataTypes/Materials/CubeMapMaterial.h"

#include "Components/Transform.h"

#include "Managers/ResourceManager.h"
#include "Managers/ConfigManager.h"

DDM::SkyBoxComponent::SkyBoxComponent()
{
	// Create the model
	auto pMesh = ResourceManager::GetInstance().LoadMesh(ConfigManager::GetInstance().GetString("SkyboxModel"));

	// Set the mesh in the meshrenderer
	SetMesh(pMesh);

	m_pMaterial = std::make_unique<CubeMapMaterial>(ConfigManager::GetInstance().GetString("SkyboxPipelineName"));

	m_pCubeMaterial = std::dynamic_pointer_cast<CubeMapMaterial>(m_pMaterial);
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

void DDM::SkyBoxComponent::SetRight(const std::string&& filepath)
{
	// Propagate to material
	m_pCubeMaterial->SetRight(filepath);
}

void DDM::SkyBoxComponent::SetLeft(const std::string& filepath)
{
	// Propagate to material
	m_pCubeMaterial->SetLeft(filepath);
}

void DDM::SkyBoxComponent::SetLeft(const std::string&& filepath)
{
	// Propagate to material
	m_pCubeMaterial->SetLeft(filepath);
}

void DDM::SkyBoxComponent::SetUp(const std::string& filepath)
{
	// Propagate to material
	m_pCubeMaterial->SetUp(filepath);
}

void DDM::SkyBoxComponent::SetUp(const std::string&& filepath)
{
	// Propagate to material
	m_pCubeMaterial->SetUp(filepath);
}

void DDM::SkyBoxComponent::SetDown(const std::string& filepath)
{
	// Propagate to material
	m_pCubeMaterial->SetDown(filepath);
}

void DDM::SkyBoxComponent::SetDown(const std::string&& filepath)
{
	// Propagate to material
	m_pCubeMaterial->SetDown(filepath);
}

void DDM::SkyBoxComponent::SetFront(const std::string& filepath)
{
	// Propagate to material
	m_pCubeMaterial->SetFront(filepath);
}

void DDM::SkyBoxComponent::SetFront(const std::string&& filepath)
{
	// Propagate to material
	m_pCubeMaterial->SetFront(filepath);
}

void DDM::SkyBoxComponent::SetBack(const std::string& filepath)
{
	// Propagate to material
	m_pCubeMaterial->SetBack(filepath);
}

void DDM::SkyBoxComponent::SetBack(const std::string&& filepath)
{
	// Propagate to material
	m_pCubeMaterial->SetBack(filepath);
}
