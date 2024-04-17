// Skybox.cpp

// File includes
#include "SkyBox.h"
#include "Utils.h"
#include "VulkanRenderer.h"
#include "Mesh.h"
#include "CubeMapMaterial.h"
#include "MeshRenderComponent.h"
#include "TextureDescriptorObject.h"

D3D::SkyBox::SkyBox(std::initializer_list<const std::string>&& filePaths)
{
	// Create cubemap material
	auto pMaterial = std::make_shared<CubeMapMaterial>(filePaths);

	// Create the model
	auto mesh = std::make_shared<Mesh>("../Resources/Models/cube.obj");

	// Set the material
	m_pMesh->SetMaterial(pMaterial);

	m_pMesh->SetMesh(mesh);
}

void D3D::SkyBox::Render()
{
	// Render the model
	m_pMesh->Render();
}
