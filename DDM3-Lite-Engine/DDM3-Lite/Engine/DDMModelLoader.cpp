// DDMModelLoader.cpp

// Header include
#include "DDMModelLoader.h"

// File includes
#include "BaseClasses/GameObject.h"
#include "Components/MeshRenderComponent.h"
#include "DataTypes/Materials/MultiMaterial.h"

DDM3::DDMModelLoader::DDMModelLoader()
{
	m_pModelLoader = std::make_unique<DDMML::DDMModelLoader>();
}

void DDM3::DDMModelLoader::LoadModel(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	std::vector<DDMML::Vertex> ddmVertices{};

	auto mesh = std::make_unique<DDMML::Mesh>();

	m_pModelLoader->LoadModel(filename, mesh.get());

	auto& ddmmlVertices = mesh->GetVertices();
	auto& ddmmlIndices = mesh->GetIndices();

	ConvertVertices(ddmmlVertices, vertices);

	indices.clear();
	indices.resize(ddmmlIndices.size());
	std::copy(ddmmlIndices.begin(), ddmmlIndices.end(), indices.begin());
}

DDM3::GameObject* DDM3::DDMModelLoader::LoadModel(const std::string& filename, GameObject* pParent)
{
	auto pDDMMLMesh{ std::make_unique<DDMML::Mesh>() };

	m_pModelLoader->LoadModel(filename, pDDMMLMesh.get());

	return SetupModel(pDDMMLMesh.get(), pParent);
}

void DDM3::DDMModelLoader::LoadScene(const std::string& path, GameObject* pParent)
{
	auto meshes = std::vector<std::unique_ptr<DDMML::Mesh>>{};
	m_pModelLoader->LoadScene(path, meshes);
	for (auto& mesh : meshes)
	{
		auto pObject = SetupModel(mesh.get(), pParent);
		pObject->SetShowImGui(true);
	}
}

void DDM3::DDMModelLoader::ConvertVertices(const std::vector<DDMML::Vertex>& ddmmlVertices, std::vector<Vertex>& vertices)
{
	DDM3::Vertex vertex{};

	vertices.clear();

	vertices.reserve(ddmmlVertices.size());
	for (auto& ddmVertex : ddmmlVertices)
	{
		vertex.color = ddmVertex.color;
		vertex.normal = ddmVertex.normal;
		vertex.pos = ddmVertex.pos;
		vertex.texCoord = ddmVertex.texCoord;
		vertex.tangent = ddmVertex.tangent;

		vertices.emplace_back(vertex);
	}
}

DDM3::GameObject* DDM3::DDMModelLoader::SetupModel(DDMML::Mesh* pDDMMLMesh, GameObject* pParent)
{
	auto pObject{ pParent->CreateNewObject(pDDMMLMesh->GetName()) };
	pObject->SetShowImGui(true);

	auto pRenderComponent{ pObject->AddComponent<DDM3::MeshRenderComponent>() };
	pRenderComponent->SetMesh(pDDMMLMesh);
	pRenderComponent->SetShowImGui(true);

	auto pMaterial = std::make_shared<DDM3::MultiMaterial>();
	
	for (auto& texture : pDDMMLMesh->GetDiffuseTextureNames())
	{
		pMaterial->AddDiffuseTexture(texture);
	}

	pRenderComponent->SetMaterial(pMaterial);

	return pObject;
}
