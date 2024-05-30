// FbxLoader.cpp

// File includes
#include "FbxLoader.h"

// Standard library includes
#include <iostream>

D3D::FbxLoader::FbxLoader()
{
	// Initialize the FBX SDK
	m_pFbxManager = FbxManager::Create();
	auto ios = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
	m_pFbxManager->SetIOSettings(ios);
}

D3D::FbxLoader::~FbxLoader()
{
	m_pFbxManager->Destroy();
}

void D3D::FbxLoader::LoadFbxModel(const std::string& path, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	FbxScene* scene = LoadScene(path);

	if (!scene)
	{
		throw std::runtime_error(path + " is not a valid file path");
	}


	int baseUvIndex{};


	// Create map to store vertices
	std::unordered_map<D3D::Vertex, uint32_t> uniqueVertices{};

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

				ConvertMesh(mesh, uniqueVertices, vertices, indices, baseUvIndex);
			}
		}
	}

	// Destroy the scene and manager
	scene->Destroy();

	SetupTangents(vertices, indices);
}

void D3D::FbxLoader::ConvertMesh(FbxMesh* pMesh,
	std::unordered_map<D3D::Vertex, uint32_t>& uniqueVertices ,std::vector<D3D::Vertex>& vertices, 
	std::vector<uint32_t>& indices, int& baseUvIndex)
{
	int numPolygons = pMesh->GetPolygonCount();

	if (numPolygons == 0)return;

	FbxVector4* controlPoints = pMesh->GetControlPoints();
	pMesh->GenerateNormals();
	pMesh->GenerateTangentsData();

	fbxSkinnedInfo skinnedInfo{};
	skinnedInfo.isSkinned = static_cast<bool>(pMesh->GetDeformerCount());

	if (skinnedInfo.isSkinned)
	{
		skinnedInfo.pSkin = static_cast<FbxSkin*>(pMesh->GetDeformer(0, FbxDeformer::eSkin));
		SetupSkin(skinnedInfo, pMesh->GetControlPointsCount());
	}

	int nextBaseUvIndex{ baseUvIndex };;

	fbxTexturedInfo texturedInfo{};
	texturedInfo.textured = static_cast<bool>(pMesh->GetElementMaterialCount());
	pMesh->GetUVSetNames(texturedInfo.uvSets);
	texturedInfo.uvSetsCount = texturedInfo.uvSets.GetCount();

	for (int polygonIndex = 0; polygonIndex < numPolygons; ++polygonIndex)
	{
		auto polygonSize = pMesh->GetPolygonSize(polygonIndex);

		for (int i = 1; i <= polygonSize - 2; i++)
		{
			if (texturedInfo.textured)
			{
				texturedInfo.uvIndex = pMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) + baseUvIndex;
			}

			if (texturedInfo.uvIndex >= nextBaseUvIndex)
			{
				nextBaseUvIndex = texturedInfo.uvIndex + 1;
			}

			HandleFbxVertex(pMesh, controlPoints, polygonIndex, 0, uniqueVertices, texturedInfo, skinnedInfo, vertices, indices);
			HandleFbxVertex(pMesh, controlPoints, polygonIndex, i, uniqueVertices, texturedInfo, skinnedInfo, vertices, indices);
			HandleFbxVertex(pMesh, controlPoints, polygonIndex, i + 1, uniqueVertices, texturedInfo, skinnedInfo, vertices, indices);
		}
	}

	baseUvIndex = nextBaseUvIndex;
}

void D3D::FbxLoader::HandleFbxVertex(FbxMesh* pMesh, FbxVector4* controlPoints, int polygonIndex, int inPolygonPosition,
	std::unordered_map<D3D::Vertex, uint32_t>& uniqueVertices,
	fbxTexturedInfo& textureInfo, fbxSkinnedInfo& skinnedInfo,
	std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	int vertexIndex = pMesh->GetPolygonVertex(polygonIndex, inPolygonPosition);

	auto controlPoint = controlPoints[vertexIndex];

	FbxVector4 normal{};
	FbxVector4 tangent{};

	pMesh->GetPolygonVertexNormal(polygonIndex, inPolygonPosition, normal);

	D3D::Vertex vertex{};

	if (textureInfo.textured)
	{
		FbxVector2 uv{};
		bool unmapped{};
		for (int i{}; i < textureInfo.uvSetsCount; i++)
		{
			if (pMesh->GetPolygonVertexUV(polygonIndex, inPolygonPosition, textureInfo.uvSets[i], uv, unmapped))
			{
				vertex.texCoord = glm::vec2{ uv[0], 1 - uv[1] };
				vertex.uvSetIndex = static_cast<float>(textureInfo.uvIndex);
			}
		}
	}

	if (skinnedInfo.isSkinned)
	{
		vertex.boneIndices = skinnedInfo.boneInfos[vertexIndex].boneIndices;
		vertex.boneWeights = skinnedInfo.boneInfos[vertexIndex].boneWeights;
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

void D3D::FbxLoader::SetupTangents(std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices)
{
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

void D3D::FbxLoader::SetupSkin(fbxSkinnedInfo& skinnedInfo, int controlPointAmount)
{
	skinnedInfo.boneInfos.resize(controlPointAmount);


	for (int boneIndex{}; boneIndex < skinnedInfo.pSkin->GetClusterCount(); boneIndex++)
	{
		auto cluster = skinnedInfo.pSkin->GetCluster(boneIndex);

		for (int j{}; j < cluster->GetControlPointIndicesCount(); j++)
		{
			int controlPointIndex = cluster->GetControlPointIndices()[j];
			float boneWeight = static_cast<float>(cluster->GetControlPointWeights()[j]);

			skinnedInfo.boneInfos[controlPointIndex].AddBone(static_cast<float>(boneIndex), boneWeight);
		}
	}
}

void D3D::FbxLoader::LoadAnimations(const std::string& path, std::vector<std::unique_ptr<AnimationClip>>& animationClips)
{
	FbxScene* pScene = LoadScene(path);

	//int animStackCount = pScene->GetSrcObjectCount<FbxAnimLayer>();

	FbxAnimLayer* test = nullptr;
	for (int i = 0; i < 5; ++i) {
		//FbxAnimStack* animStack = (FbxAnimStack*)pScene->GetSrcObject<FbxAnimStack>(i);
		// Use animStack
	}


	pScene->Destroy();
}

FbxScene* D3D::FbxLoader::LoadScene(const std::string& path)
{
	auto pFbxImporter = FbxImporter::Create(m_pFbxManager, "importer");
	// Import the FBX file
	const char* filename = path.c_str();
	if (!pFbxImporter->Initialize(filename, -1, m_pFbxManager->GetIOSettings())) {
		throw std::runtime_error(path + " is not a valid file path");
	}

	// Create a scene and import it
	FbxScene* scene = FbxScene::Create(m_pFbxManager, "Scene");
	pFbxImporter->Import(scene);

	pFbxImporter->Destroy();

	return scene;
}

