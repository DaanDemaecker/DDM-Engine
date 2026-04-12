// DDMModelLoader.cpp

// Header include
#include "DDMModelLoader.h"

// File includes
#include "DDM-Engine/BaseClasses/GameObject.h"
#include "DDM-Engine/EngineComponents/MeshRenderer.h"
#include "DDM-Engine/DataTypes/Materials/MultiMaterial.h"

DDM::DDMModelLoader::DDMModelLoader()
{
	m_pModelLoader = std::make_unique<DDMML::DDMModelLoader>();
}

void DDM::DDMModelLoader::LoadModel(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	std::vector<DDMML::Vertex> ddmVertices{};

	auto mesh = m_pModelLoader->LoadModel(filename, "Placeholder");

	auto& ddmmlVertices = mesh->GetVertices();
	auto& ddmmlIndices = mesh->GetIndices();

	ConvertVertices(ddmmlVertices, vertices);

	indices.clear();
	indices.resize(ddmmlIndices.size());
	std::copy(ddmmlIndices.begin(), ddmmlIndices.end(), indices.begin());
}

DDM::GameObject* DDM::DDMModelLoader::LoadModel(const std::string& filename, GameObject* pParent)
{
	auto pDDMMLMesh = m_pModelLoader->LoadModel(filename, pParent->GetName());

	return SetupModel(pDDMMLMesh.get(), pParent);
}

std::unique_ptr<DDMML::Mesh> DDM::DDMModelLoader::LoadModel(const std::string& fileName, const std::string& name)
{
	return m_pModelLoader->LoadModel(fileName, name);
}

void DDM::DDMModelLoader::LoadTexturedScene(const std::string& path, GameObject* pParent)
{
	auto pMeshes = std::vector<std::unique_ptr<DDMML::Mesh>>{};
	m_pModelLoader->LoadScene(path, pMeshes);
	for (auto& mesh : pMeshes)
	{
		auto pObject = SetupModel(mesh.get(), pParent);
	}
}

void DDM::DDMModelLoader::LoadScene(const std::string& path, std::vector<std::unique_ptr<DDMML::Mesh>>& pMeshes)
{
	m_pModelLoader->LoadScene(path, pMeshes);
}

void DDM::DDMModelLoader::ConvertVertices(const std::vector<DDMML::Vertex>& ddmmlVertices, std::vector<Vertex>& vertices)
{
	DDM::Vertex vertex{};

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

DDM::GameObject* DDM::DDMModelLoader::SetupModel(DDMML::Mesh* pDDMMLMesh, GameObject* pParent)
{
	auto pObject{ pParent->CreateNewObject(pDDMMLMesh->GetName()) };
	pObject->SetShowImGui(true);

	auto pRenderComponent{ pObject->AddComponent<DDM::MeshRenderComponent>() };
	pRenderComponent->SetMesh(pDDMMLMesh);
	pRenderComponent->SetShowImGui(true);

	auto pMaterial = std::make_shared<DDM::MultiMaterial>();
	
	for (auto& texture : pDDMMLMesh->GetDiffuseTextureNames())
	{
		pMaterial->AddDiffuseTexture(texture);
	}

	pRenderComponent->SetMaterial(pMaterial);

	return pObject;
}
