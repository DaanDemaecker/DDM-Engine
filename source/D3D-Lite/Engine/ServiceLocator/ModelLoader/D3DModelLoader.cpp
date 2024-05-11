// D3DModelLoader.cpp

// File includes
#include "D3DModelLoader.h"
#include "../../../Includes/TinyObjLoaderIncludes.h"
#include "../../../Utils/Utils.h"

// Standard library includes
#include <stdexcept>
#include <iostream>

void D3D::D3DModelLoader::LoadModel(const std::string& path, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	// Get the extension of the file
	auto extension{std::move(Utils::GetExtension(path))};

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
	// Clear the vectors in case they aren't empty
	vertices.clear();
	indices.clear();

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

	// Loop trough every shape that was read from the file
	for (const auto& shape : shapes)
	{
		// Loop trough all indices in current shape
		for (const auto& index : shape.mesh.indices)
		{
			// Create empty vertex
			D3D::Vertex vertex{};

			// Add position to vertex
			vertex.pos = {
				attrib.vertices[static_cast<uint64_t>(3) * index.vertex_index],
				attrib.vertices[static_cast<uint64_t>(3) * index.vertex_index + static_cast<uint64_t>(1)],
				attrib.vertices[static_cast<uint64_t>(3) * index.vertex_index + static_cast<uint64_t>(2)]
			};

			if (index.texcoord_index >= 0 && index.texcoord_index < static_cast<int>(attrib.texcoords.size() / 2))
			{
				// Add UV coords to vertex
				vertex.texCoord = {
					attrib.texcoords[static_cast<uint64_t>(2) * index.texcoord_index],
					1.0f - attrib.texcoords[static_cast<uint64_t>(2) * index.texcoord_index + 1]
				};
			}

			if (index.normal_index >= 0 && index.normal_index < +static_cast<int>(attrib.texcoords.size() / 3))
			{
				// Add normal to vertex
				vertex.normal = {
					attrib.normals[static_cast<uint64_t>(3) * index.normal_index],
					attrib.normals[static_cast<uint64_t>(3) * index.normal_index + static_cast<uint64_t>(1)],
					attrib.normals[static_cast<uint64_t>(3) * index.normal_index + static_cast<uint64_t>(2)]
				};
			}

			// Add color to vertex
			vertex.color = { 1, 1, 1 };

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

void D3D::D3DModelLoader::LoadFbx(const std::string& path, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	// Initialize the FBX SDK
	FbxManager* manager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);

	// Create an importer
	FbxImporter* importer = FbxImporter::Create(manager, "");

	// Import the FBX file
	const char* filename = path.c_str();
	if (!importer->Initialize(filename, -1, manager->GetIOSettings())) {
		std::cerr << "Failed to initialize importer: " << importer->GetStatus().GetErrorString() << std::endl;
		return;
	}

	// Create a scene and import it
	FbxScene* scene = FbxScene::Create(manager, "Scene");
	importer->Import(scene);
	importer->Destroy();

	// Traverse the scene to find nodes containing the car model
	FbxNode* root = scene->GetRootNode();
	if (root) {
		for (int i = 0; i < root->GetChildCount(); i++) {
			FbxNode* child = root->GetChild(i);
			// Check if the node contains mesh data
			if (child->GetNodeAttribute() && child->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				// Extract mesh data
				FbxMesh* mesh = child->GetMesh();

				ConvertMesh(mesh, vertices, indices);
			}
		}
	}

	// Destroy the scene and manager
	scene->Destroy();
	manager->Destroy();
}

void D3D::D3DModelLoader::ConvertMesh(FbxMesh* pMesh, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	int numPolygons = pMesh->GetPolygonCount();
	FbxVector4* controlPoints = pMesh->GetControlPoints();
	pMesh->GenerateNormals();
	pMesh->GenerateTangentsData();

	vertices.clear();
	indices.clear();

	// Create map to store vertices
	std::unordered_map<D3D::Vertex, uint32_t> uniqueVertices{};

	for (int polygonIndex = 0; polygonIndex < numPolygons; ++polygonIndex)
	{
		auto polygonSize = pMesh->GetPolygonSize(polygonIndex);

		for (int i = 1; i <= polygonSize - 2; i++)
		{
			HandleFbxVertex(pMesh, controlPoints, polygonIndex, 0, uniqueVertices, vertices, indices);
			HandleFbxVertex(pMesh, controlPoints, polygonIndex, i, uniqueVertices, vertices, indices);
			HandleFbxVertex(pMesh, controlPoints, polygonIndex, i + 1, uniqueVertices, vertices, indices);
		}
	}
}

void D3D::D3DModelLoader::HandleFbxVertex(FbxMesh* pMesh, FbxVector4* controlPoints, int polygonIndex, int inPolygonPosition,
	std::unordered_map<D3D::Vertex, uint32_t>& uniqueVertices,
	std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	int vertexIndex = pMesh->GetPolygonVertex(polygonIndex, inPolygonPosition);

	auto controlPoint = controlPoints[vertexIndex];


	D3D::Vertex vertex{};

	vertex.pos = glm::vec3{ controlPoint[0], controlPoint[1], controlPoint[2] };
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

