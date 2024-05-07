// Skybox.cpp

// File includes
#include "SkyBoxComponent.h"
#include "../Utils/Utils.h"
#include "../Vulkan/VulkanRenderer.h"
#include "../DataTypes/Mesh.h"
#include "../DataTypes/DescriptorObjects/TextureDescriptorObject.h"	
#include "../DataTypes/Materials/CubeMapMaterial.h"
#include "TransformComponent.h"

D3D::SkyBoxComponent::SkyBoxComponent()
{
	// Create the model
	auto pMesh = std::make_shared<Mesh>("Resources/Models/cube.obj");

	SetMesh(pMesh);
}

void D3D::SkyBoxComponent::EarlyUpdate()
{
	if (m_ShouldCreateDescriptorSets)
	{
		CreateDescriptorSets();
		m_ShouldCreateDescriptorSets = false;
	}
}

void D3D::SkyBoxComponent::LoadSkybox(std::initializer_list<const std::string>&& filePaths)
{
	// Create cubemap material
	m_pMaterial = std::make_shared<D3D::CubeMapMaterial>(filePaths);
}

void D3D::SkyBoxComponent::RenderSkyBox()
{
	if (m_pMesh == nullptr)
		return;

	auto frame{ D3D::VulkanRenderer::GetInstance().GetCurrentFrame()};

	UpdateUniformBuffer(frame);

	m_pMesh->Render(GetPipeline(), &m_DescriptorSets[frame]);
}

void D3D::SkyBoxComponent::Render()
{
}
