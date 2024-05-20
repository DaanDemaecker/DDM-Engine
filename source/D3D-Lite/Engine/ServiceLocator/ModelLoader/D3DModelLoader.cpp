// D3DModelLoader.cpp

// File includes
#include "D3DModelLoader.h"
#include "../../../Includes/TinyObjLoaderIncludes.h"
#include "../../../Utils/Utils.h"

// Standard library includes
#include <stdexcept>
#include <iostream>

D3D::D3DModelLoader::D3DModelLoader()
{
	// Initialize the FBX SDK
	m_pFbxManager = FbxManager::Create();
	auto ios = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
	m_pFbxManager->SetIOSettings(ios);
}

D3D::D3DModelLoader::~D3DModelLoader()
{
	m_pFbxManager->Destroy();
}

void D3D::D3DModelLoader::LoadModel(const std::string& path, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	// Get the extension of the file
	auto extension{std::move(Utils::GetExtension(path))};

	vertices.clear();
	indices.clear();

	if (extension == "obj")
	{
		// If the extension is .obj, use obj loader
		LoadObj(path, vertices, indices);
	}
	else if (extension == "fbx")
	{
		// If the extension is .fbx, use fbx sdk to load in model
		LoadFbx(path, vertices, indices);
	}
	else
	{
		// If the extension can't be handled, warn user
		std::cout << extension << " is not a supported model format \n";
	}
}

void D3D::D3DModelLoader::LoadObj(const std::string& path, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	// Create needed objects to read in .obj file
	tinyobj::attrib_t attrib{};
	std::vector<tinyobj::shape_t> shapes{};
	std::vector<tinyobj::material_t> materials{};

	// Create objects for error throwing
	std::string err;

	// Read file, returned false, throw error
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str()))
	{
		throw std::runtime_error(err);
	}

	// Create map to store vertices
	std::unordered_map<D3D::Vertex, uint32_t> uniqueVertices{};

	// Loop through every shape that was read from the file
	for (const auto& shape : shapes)
	{
		// Loop through all indices in current shape
		for (const auto& index : shape.mesh.indices)
		{
			// Create empty vertex
			D3D::Vertex vertex{};

			// Add position to vertex
			vertex.pos = {
				attrib.vertices[3 * index.vertex_index],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			if (index.texcoord_index >= 0 && static_cast<size_t>(index.texcoord_index * 2 + 1) < attrib.texcoords.size())
			{
				// Add UV coords to vertex
				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			if (index.normal_index >= 0 && static_cast<size_t>(index.normal_index * 3 + 2) < attrib.normals.size())
			{
				// Add normal to vertex
				vertex.normal = {
					attrib.normals[3 * index.normal_index],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
			}
			else
			{
				// Default normal if not provided
				vertex.normal = { 0.0f, 0.0f, 0.0f };
			}

			// Add color to vertex
			vertex.color = { 1.0f, 1.0f, 1.0f };

			// If vertex isn't in uniqueVertices vector, add it
			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			// Add index to indices vector
			indices.push_back(uniqueVertices[vertex]);
		}
	}

	// After all vertices are added loop through them to calculate the tangents
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		// Get the indices of the current triangle
		uint32_t index0 = indices[i];
		uint32_t index1 = indices[i + 1];
		uint32_t index2 = indices[i + 2];

		// Get the vertices associated with this triangle
		D3D::Vertex& v0 = vertices[index0];
		D3D::Vertex& v1 = vertices[index1];
		D3D::Vertex& v2 = vertices[index2];

		// Get 2 edges of this triangle
		glm::vec3 edge1 = v1.pos - v0.pos;
		glm::vec3 edge2 = v2.pos - v0.pos;

		// Get the difference in UV over these edges
		glm::vec2 deltaUV1 = v1.texCoord - v0.texCoord;
		glm::vec2 deltaUV2 = v2.texCoord - v0.texCoord;

		// Calculate the scaling factor for normalizing the vector
		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

		// Calculate the tangent
		glm::vec3 tangent = (edge1 * deltaUV2.y - edge2 * deltaUV1.y) * r;

		// Add the tangent to the 3 vertices
		v0.tangent += tangent;
		v1.tangent += tangent;
		v2.tangent += tangent;
	}

	// Normalize the tangents
	for (auto& vertex : vertices)
	{
		vertex.tangent = glm::normalize(vertex.tangent);
	}
}

void D3D::D3DModelLoader::LoadFbx(const std::string& path, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	auto pFbxImporter = FbxImporter::Create(m_pFbxManager, "importer");
	// Import the FBX file
	const char* filename = path.c_str();
	if (!pFbxImporter->Initialize(filename, -1, m_pFbxManager->GetIOSettings())) {
		std::cerr << "Failed to initialize importer: " << pFbxImporter->GetStatus().GetErrorString() << std::endl;
		return;
	}

	// Create a scene and import it
	FbxScene* scene = FbxScene::Create(m_pFbxManager, "Scene");
	pFbxImporter->Import(scene);

	int baseUvIndex{};

	// Traverse the scene to find nodes containing the car model
	FbxNode* root = scene->GetRootNode();
	if (root)
	{
		for (int i = 0; i < root->GetChildCount(); i++)
		{
			FbxNode* child = root->GetChild(i);
			// Check if the node contains mesh data
			if (child->GetNodeAttribute() && child->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				// Extract mesh data
				FbxMesh* mesh = child->GetMesh();

				ConvertMesh(mesh, vertices, indices, baseUvIndex);
			}
		}
	}

	// Destroy the scene and manager
	scene->Destroy();
	pFbxImporter->Destroy();


	// After all vertices are added loop trought them to calculate the tangents
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		// Get the the indices of the current triangle
		uint32_t index0 = indices[i];
		uint32_t index1 = indices[i + 1];
		uint32_t index2 = indices[i + 2];

		// Get the vertices asociated with this triangle
		D3D::Vertex& v0 = vertices[index0];
		D3D::Vertex& v1 = vertices[index1];
		D3D::Vertex& v2 = vertices[index2];

		// Get 2 edges of this triangle
		glm::vec3 edge1 = v1.pos - v0.pos;
		glm::vec3 edge2 = v2.pos - v0.pos;

		// Get the difference in UV over these edges
		glm::vec2 deltaUV1 = v1.texCoord - v0.texCoord;
		glm::vec2 deltaUV2 = v2.texCoord - v0.texCoord;

		// Calculate the scaling factor for normalizing the vector
		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

		// Calculate the tangent
		glm::vec3 tangent = (edge1 * deltaUV2.y - edge2 * deltaUV1.y) * r;

		// Add the tangent to the 3 vertices
		v0.tangent += tangent;
		v1.tangent += tangent;
		v2.tangent += tangent;
	}
}

void D3D::D3DModelLoader::ConvertMesh(FbxMesh* pMesh, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices, int& baseUvIndex)
{
	int numPolygons = pMesh->GetPolygonCount();
	FbxVector4* controlPoints = pMesh->GetControlPoints();
	pMesh->GenerateNormals();
	pMesh->GenerateTangentsData();

	std::cout << "Geometry weigted map count: " << pMesh->GetDestinationGeometryWeightedMapCount() << std::endl;

	// Create map to store vertices
	std::unordered_map<D3D::Vertex, uint32_t> uniqueVertices{};

	FbxStringList uvSets{};
	pMesh->GetUVSetNames(uvSets);

	int nextBaseUvIndex{baseUvIndex};;

	bool textured{ static_cast<bool>(pMesh->GetElementMaterialCount())};

	for (int polygonIndex = 0; polygonIndex < numPolygons; ++polygonIndex)
	{
		auto polygonSize = pMesh->GetPolygonSize(polygonIndex);

		for (int i = 1; i <= polygonSize - 2; i++)
		{
			int uvIndex{};
			if (textured)
			{
				int uvIndex = pMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) + baseUvIndex;
			}
			
			if (uvIndex >= nextBaseUvIndex)
			{
				nextBaseUvIndex = uvIndex + 1;
			}

			HandleFbxVertex(pMesh, controlPoints, polygonIndex, 0, uniqueVertices, textured, uvIndex, uvSets, vertices, indices);
			HandleFbxVertex(pMesh, controlPoints, polygonIndex, i, uniqueVertices, textured, uvIndex, uvSets, vertices, indices);
			HandleFbxVertex(pMesh, controlPoints, polygonIndex, i + 1, uniqueVertices, textured, uvIndex, uvSets, vertices, indices);
		}
	}

	baseUvIndex = nextBaseUvIndex;
}

void D3D::D3DModelLoader::HandleFbxVertex(FbxMesh* pMesh, FbxVector4* controlPoints, int polygonIndex, int inPolygonPosition,
	std::unordered_map<D3D::Vertex, uint32_t>& uniqueVertices, bool textured, int uvIndex, FbxStringList uvSets,
	std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	int vertexIndex = pMesh->GetPolygonVertex(polygonIndex, inPolygonPosition);

	auto controlPoint = controlPoints[vertexIndex];

	FbxVector4 normal{};
	FbxVector4 tangent{};

	pMesh->GetPolygonVertexNormal(polygonIndex, inPolygonPosition, normal);

	D3D::Vertex vertex{};

	if (textured)
	{
		FbxVector2 uv{};
		bool unmapped{};
		for (int i{}; i < uvSets.GetCount(); i++)
		{
			if (pMesh->GetPolygonVertexUV(polygonIndex, inPolygonPosition, uvSets[i], uv, unmapped))
			{
				vertex.texCoord = glm::vec2{ uv[0], 1 - uv[1] };
				vertex.uvSetIndex = uvIndex;
			}
		}
	}

	vertex.pos = glm::vec3{ controlPoint[0], controlPoint[1], controlPoint[2] };
	vertex.normal = glm::vec3{ normal[0], normal[1], normal[2] };
	vertex.color = glm::vec3{ 1, 1, 1 };

	// If vertex isn't in uniqueVertices vector, add it
	if (!uniqueVertices.contains(vertex))
	{
		uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
		vertices.push_back(vertex);
	}

	// Add index to indices vector
	indices.push_back(uniqueVertices[vertex]);
}

