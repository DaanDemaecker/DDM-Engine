// DDMModelLoader.cpp

// Header include
#include "DDMModelLoader.h"

// File includes
#include "BaseClasses/GameObject.h"

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

void DDM3::DDMModelLoader::LoadModel(const std::string& filename, GameObject* pParent)
{

}

void DDM3::DDMModelLoader::LoadScene(const std::string& path, GameObject* pParent)
{
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
